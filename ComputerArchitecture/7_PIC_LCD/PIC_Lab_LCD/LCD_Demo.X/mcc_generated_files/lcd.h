/* 
 * File:   lcd.h
 * Author: Sina Radmehr
 * Note: Hope you enjoy using this library :)
 * Created on December 1, 2017, 3:15 PM
 */

#ifndef LCD_H
#define	LCD_H
#include "mcc.h"

#ifdef	__cplusplus
extern "C" {
#endif
/**
    Header File for Module lcd.c
  File Name:
    lcd.h
  Summary:
    This file contains the function prototypes for LCD.
*/

/**
 Section: Macro Definitions
*/
// set up the timing for the LCD delays
#define LCD_delay           5     // ~5mS
#define LCD_Startup         15    // ~15mS

// Command set for Hitachi 44780U LCD display controller
// use these with LCDPutCmd(uint8_t ch)
#define LCD_CLEAR           0x01
#define LCD_HOME            0x02
#define LCD_CURSOR_BACK     0x10
#define LCD_CURSOR_FWD      0x14
#define LCD_PAN_LEFT        0x18
#define LCD_PAN_RIGHT       0x1C
#define LCD_CURSOR_OFF      0x0C
#define LCD_CURSOR_ON       0x0E
#define LCD_CURSOR_BLINK    0x0F
#define LCD_CURSOR_LINE2    0xC0

// display controller setup commands from page 46 of Hitachi datasheet
#define FUNCTION_SET        0x28                         // 4 bit interface, 2 lines, 5x8 font
#define ENTRY_MODE          0x06                         // increment mode
#define DISPLAY_SETUP       0x0C                         // display on, cursor off, blink offd

// Other syntaxes:
#define LCDLine1()          LCDPutCmd(LCD_HOME)          // legacy support
#define LCDLine2()          LCDPutCmd(LCD_CURSOR_LINE2)  // legacy support
#define shift_cursor()      LCDPutCmd(LCD_CURSOR_FWD)    // legacy support
#define cursor_on()         LCDPutCmd(LCD_CURSOR_ON)     // legacy support
#define DisplayClr()        LCDPutCmd(LCD_CLEAR)         // Legacy support

// single bit for selecting command register or data register
#define instr        0
#define data         1

// These #defines create the pin connections to the LCD in case they are changed on a future demo board
#define LCD_PORT     LATB
#define LCD_PWR      LATBbits.LATB3                      // LCD power pin
#define LCD_BACKLIGHT LATBbits.LATB4
#define LCD_EN       LATBbits.LATB7                     // LCD enable
#define LCD_RW       LATBbits.LATB6                      // LCD read/write line
#define LCD_RS       LATBbits.LATB5                      // LCD register select line

#define NB_LINES    2                                   // Number of display lines
#define NB_COL      16                                  // Number of characters per line

// Function prototypes
/**
  @Summary
    initialize the LCD module

  @Description
    This routine initializes the LCD driver.
    This routine must be called before any other LCD routine is called.

  @Preconditions
    None

  @Param
    None

  @Returns
    None
*/
void LCD_Initialize(void);
 
 /**
  @Summary
    Writes character to LCD at current cursor position

  @Description
    This function displays the specified ASCII character at current position on the LCD

  @Preconditions
    None

  @Param
    ASCII character to be displayed

  @Returns
    None
*/
void LCDPutChar(uint8_t ch);

/**
  @Summary
    send an ASCII command to the LCD in instruction mode

  @Description
    This routine writes character to LCD command register

  @Preconditions
    None

  @Param
    ASCII command 

  @Returns
    None
*/
void LCDPutCmd(uint8_t ch);

/**
  @Summary
    display a string

  @Description
    This routine writes string to LCD at current cursor position

  @Preconditions
    None

  @Param
    Pointer to string

  @Returns
    None
*/
void LCDPutStr(const char *);                         

/**
  @Summary
    Fuction to write a nibble

  @Description
    This function writes the specified nibble to the LCD.

  @Preconditions
    None

  @Param
    data byte to be written to display

  @Returns
    None
*/
void LCDWriteNibble(uint8_t ch,uint8_t rs);

/**
  @Summary
    Initialization routine that takes inputs from the EUSART GUI.

  @Description
    This function positions the cursor at the specified Line and column.

  @Preconditions
    None

  @Param
    Column and line at which the cursor should be positioned at

  @Returns
    None

  @Comment
      0 <= pos <= 15
      0 <= ln <= 1
*/
void LCDGoto(uint8_t pos, uint8_t ln);
    
void __delay_ms(unsigned long delay);
void LCDClear(void);



#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

