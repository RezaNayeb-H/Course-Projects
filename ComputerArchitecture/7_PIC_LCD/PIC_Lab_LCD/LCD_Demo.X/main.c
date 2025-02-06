/* 
 * File:   main.c
 * Author: Sina Radmehr
 * Note: This file works as a test
 * Created on December 9, 2017, 4:14 PM
 */
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/lcd.h"

int main(void)
{
    // initialize the device
    // Use ctrl+click to see inside of the functions
    SYSTEM_Initialize();
    LCD_Initialize();
    // remember to clear lcd before first usage
    LCDClear();
    //LCDPutCmd(LCD_CURSOR_ON);
    int i=0;
    while (1)
    {
        LCDPutCmd(LCD_HOME);
        LCDGoto(i,0);
        LCDPutChar('0'+i);
        __delay_ms(500);
        i++;
        // Add your application code
        if(i==5){LCDClear();}
        if(i==10){
            i=0;
            LCDGoto(0,1);
            LCDPutStr("<<Hello World!>>");
        }
    }

    return -1;
}
/**
 End of File
*/