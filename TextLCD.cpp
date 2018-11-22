/* mbed TextLCD Library, for a 4-bit LCD based on HD44780
 * Copyright (c) 2007-2010, sford, http://mbed.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "TextLCD.h"
#include "mbed.h"

TextLCD::TextLCD(PinName rs, PinName rw, PinName e, PinName d0, PinName d1,
                 PinName d2, PinName d3, LCDType type) : _rs(rs), _rw(rw),
        _e(e), _d(d0, d1, d2, d3), _type(type) {
    _rs = 0;            // command mode
    _rw = 0;
    _e  = 0;            
    _d.output();        // data out

    wait(0.05);        // Wait 50ms to ensure powered up

    // send "Display Settings" 3 times (Only top nibble of 0x30 as we've got 4-bit bus)
    for (int i=0; i<3; i++) {
        _e = 1;
        __nop();
        _d = 0x3;
        __nop();
        _e = 0;
        wait(0.004f); // 4ms
       }
    _e = 1;
    __nop();
    _d = 0x2;           // 4 Bit mode
    __nop(); 
    _e = 0;
    
    writeCommand(0x28); // Function set 4 Bit, 2Line, 5*7
    writeCommand(0x08); // Display off
    writeCommand(0x01); // clear Display
    writeCommand(0x04); // cursor right, Display is not shifted
    writeCommand(0x0C); // Display on , Cursor off 
}

void TextLCD::character(int column, int row, int c) {
    int a = address(column, row);
    writeCommand(a);
    writeData(c);
}

void TextLCD::cls() {
    writeCommand(0x01); // cls, and set cursor to 0
    locate(0, 0);
}

void TextLCD::locate(int column, int row) {
    _column = column;
    _row = row;
}



int TextLCD::_putc(int value) {
    if (value == '\n') {
        _column = 0;
        _row++;
        if (_row >= rows()) {
            _row = 0;
        }
    } else {
        character(_column, _row, value);
        _column++;
        if (_column >= columns()) {
            _column = 0;
            _row++;
            if (_row >= rows()) {
                _row = 0;
            }
        }
    }
    return value;
}

int TextLCD::_getc() {
    int a = address(_column, _row);
    writeCommand(a);
    return (readData());
}

void TextLCD::writeByte(int value) {
    _e = 1;
    __nop();    
    _d = value >> 4;
    __nop();
    _e = 0;
    __nop();
    _e = 1;
    __nop();
    _d = value >> 0;
    __nop();
    _e = 0;
}

void TextLCD::writeCommand(int command) {
    waitBusy();  // check if display is ready 
    _rs = 0;
    writeByte(command);
}

int TextLCD::readData(){
    int input;
    waitBusy();
    _rw = 1;
    _rs = 1;
    __nop();
    _d.input();  // switch Data port to input
    _e = 1;
    __nop();
    input = _d.read() << 4; // high nibble
    _e = 0;
    __nop();
    _e = 1;
    __nop();
    input = input | _d.read(); // low nibble
    _e = 0;   
    return (input);
}

 void TextLCD::waitBusy(){
    int input;
    _rw = 1;
    _rs = 0;
    __nop();
    _d.input();      // switch Data port to input
    do{ 
        _e = 1;
        __nop();
        input = _d.read();              
        _e = 0;
        __nop();
        _e = 1;
        __nop();
        _e = 0;
       }while((0x8 & input) == 0x8);  // wait until display is ready
    _rw = 0;
    _d.output();      // switch port back to output  
 }

void TextLCD::writeData(int data) {
    waitBusy();
    _rs = 1;
    writeByte(data);
}


// set user defined char 
void  TextLCD::writeCGRAM(int address, int pattern[8]){
    int i;
    address = address & 0x07;  //max 8 char
    for(i=0;i<8;i++){
        waitBusy();  // check if display is ready 
        _rs = 0;
        writeByte(0x40 + address * 8 + i);
        writeData(pattern[i]);
        }
}   
        

int TextLCD::address(int column, int row) {
    switch (_type) {
        case LCD20x4:
            switch (row) {
                case 0:
                    return 0x80 + column;
                case 1:
                    return 0xc0 + column;
                case 2:
                    return 0x94 + column;
                case 3:
                    return 0xd4 + column;
            }
        case LCD16x2B:
            return 0x80 + (row * 40) + column;
        case LCD16x2:
        case LCD20x2:
        default:
            return 0x80 + (row * 0x40) + column;
    }
}

int TextLCD::columns() {
    switch (_type) {
        case LCD20x4:
        case LCD20x2:
            return 20;
        case LCD16x2:
        case LCD16x2B:
        default:
            return 16;
    }
}

int TextLCD::rows() {
    switch (_type) {
        case LCD20x4:
            return 4;
        case LCD16x2:
        case LCD16x2B:
        case LCD20x2:
        default:
            return 2;
    }
}
