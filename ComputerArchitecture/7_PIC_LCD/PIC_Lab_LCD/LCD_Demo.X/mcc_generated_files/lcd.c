/*
  Section: Included Files
*/
#include <xc.h>
#include "lcd.h"

void __delay_ms(unsigned long delay){
    unsigned long i=0; 
    for(i=0;i<(unsigned long)(delay*(_XTAL_FREQ/150000.0));i++);
}
void LCDClear(){
    LCDPutCmd(LCD_CLEAR);
}

void LCD_Initialize()
{
    // clear latches before enabling TRIS bits
    //LCD_PORT = 0;
    //LATA=0;
    //LATB=0;
    ///////////////////
    //TRISA = 0x0000;
    //TRISB = 0x0000;
    ///////////////////
    // power up the LCD
    LCD_PWR=0;
    __delay_ms(20);
    LCD_PWR = 1;
    LCD_BACKLIGHT = 1;
    // required by display controller to allow power to stabilize
    __delay_ms(LCD_Startup);

    // required by display initialization
    LCDPutCmd(0x32);

    // set interface size, # of lines and font
    LCDPutCmd(FUNCTION_SET);

    // turn on display and sets up cursor
    LCDPutCmd(DISPLAY_SETUP);
    
    LCDPutCmd(LCD_CLEAR);

    // set cursor movement direction
    LCDPutCmd(ENTRY_MODE);

}


void LCDWriteNibble(uint8_t ch,uint8_t rs)
{
    // always send the upper nibble
    ch = (ch >> 4);
    
    // mask off the nibble to be transmitted
    ch = (ch & 0x0F);

    // clear the lower half of LCD_PORT
    LCD_PORT = (LCD_PORT & 0xF0FF);

    // move the nibble onto LCD_PORT
    LCD_PORT = (LCD_PORT | (ch<<8));

    // set data/instr bit to 0 = insructions; 1 = data
    LCD_RS = rs;

    // RW - set write mode
    LCD_RW = 0;

    // set up enable before writing nibble
    LCD_EN = 1;
    //__delay_ms(10);
    // turn off enable after write of nibble
    LCD_EN = 0;
    //__delay_ms(10);
}

void LCDPutChar(uint8_t ch)
{
    __delay_ms(LCD_delay);

    //Send higher nibble first
    LCDWriteNibble(ch,data);

    //get the lower nibble
    ch = (ch << 4);

    // Now send the low nibble
    LCDWriteNibble(ch,data);
}

    
void LCDPutCmd(uint8_t ch)
{
    __delay_ms(LCD_delay);

    //Send the higher nibble
    LCDWriteNibble(ch,instr);

    //get the lower nibble
    ch = (ch << 4);

    //__delay_ms(1);

    //Now send the lower nibble
    LCDWriteNibble(ch,instr);
}

 
void LCDPutStr(const char *str)
{
    uint8_t i=0;
    
    // While string has not been fully traveresed
    while (str[i])
    {
        // Go display current char
        LCDPutChar(str[i++]);
    }
    
}

void LCDGoto(uint8_t pos,uint8_t ln)
{
    // if incorrect line or column
    if ((ln > (NB_LINES-1)) || (pos > (NB_COL-1)))
    {
        // Just do nothing
        return;
    }

    // LCD_Goto command
    LCDPutCmd((ln == 1) ? (0xC0 | pos) : (0x80 | pos));

    // Wait for the LCD to finish
    __delay_ms(LCD_delay);
}






