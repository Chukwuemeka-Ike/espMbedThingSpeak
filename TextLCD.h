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

#ifndef MBED_TEXTLCD_H
#define MBED_TEXTLCD_H

#include "mbed.h"

/** A TextLCD interface for driving 4-bit HD44780-based LCDs
 *
 * Currently supports 16x2, 20x2 and 20x4 panels
 *
 * @code
 * #include "mbed.h"
 * #include "TextLCD.h"
 * 
 * TextLCD lcd(p10, p12, ,p13, p15, p16, p29, p30); // rs, rw, e, d0-d3
 * 
 * int main() {
 *     lcd.printf("Hello mbed\n");
 *
 * //define user chars
 *  int pattern[8];
 *  int pattern1[8];
 *  pattern[0] = 1;              //     *
 *  pattern[1] = 3;              //    **
 *  pattern[2] = 5;              //   * *
 *  pattern[3] = 9;              //  *  *
 *  pattern[4] = 0x11;           // *   *     
 *  pattern[5] = 0x19;           // **  * 
 *  pattern[6] = 0x1d;           // *** *
 *  pattern[7] = 0x1f;           // *****
 *  
 *  pattern1[0] = 0x10;          // *
 *  pattern1[1] = 0x18;          // **
 *  pattern1[2] = 0x14;          // * *
 *  pattern1[3] = 0x12;          // *  *
 *  pattern1[4] = 0x11;          // *   *
 *  pattern1[5] = 0x13;          // *  **
 *  pattern1[6] = 0x17;          // * ***
 *  pattern1[7] = 0x1f;          // *****
 * 
 *  lcd.writeCGRAM(0, pattern);
 *  lcd.writeCGRAM(1, pattern1);
 *  
 *  lcd.locate(15,0);
 *  lcd.putc(0);   // user pattern 0
 *  lcd.putc(1);   // user pattern 1   
 * }
 * @endcode
 */
class TextLCD : public Stream {
public:

    /** LCD panel format */
    enum LCDType {
        LCD16x2     /**< 16x2 LCD panel (default) */
        , LCD16x2B  /**< 16x2 LCD panel alternate addressing */
        , LCD20x2   /**< 20x2 LCD panel */
        , LCD20x4   /**< 20x4 LCD panel */
    };

    /** Create a TextLCD interface
     *
     * @param rs    Instruction/data control line
     * @param e     Enable line (clock)
     * @param rw    read / write
     * @param d0-d3 Data lines
     * @param type  Sets the panel size/addressing mode (default = LCD16x2)
     */
    TextLCD(PinName rs, PinName rw, PinName e, PinName d0, PinName d1, PinName d2, PinName d3, LCDType type = LCD16x2);

#if DOXYGEN_ONLY
    /** Write a character to the LCD
     *
     * @param c The character to write to the display
     */
    int putc(int c);

    /** Write a formated string to the LCD
     *
     * @param format A printf-style format string, followed by the
     *               variables to use in formating the string.
     */
    int printf(const char* format, ...);
#endif

    /** Locate to a screen column and row
     *
     * @param column  The horizontal position from the left, indexed from 0
     * @param row     The vertical position from the top, indexed from 0
     */
    void locate(int column, int row);

    /** Clear the screen and locate to 0,0 */
    void cls();

    int rows();
    int columns();
    
    /** write a user defined char
     *
     * @param address  The user defined char (0-7)
     * @param pattern[8] bit pattern 5*8 of char
     */ 
    void writeCGRAM(int address, int pattern[8]);
    
    /** Get the char at the current position
     * 
     * int getc() 
     */
    
protected:

    // Stream implementation functions
    virtual int _putc(int value);
    virtual int _getc();

    int address(int column, int row);
    void character(int column, int row, int c);
    void writeByte(int value);
    void writeCommand(int command);
    void writeData(int data);
    int readData();
    void waitBusy();
    DigitalOut _rs, _e, _rw;
    BusInOut _d;
    LCDType _type;

    int _column;
    int _row;
};

#endif
