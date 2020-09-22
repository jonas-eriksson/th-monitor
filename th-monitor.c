/*************************************************************************
* FILENAME :        th-monitor.c             
*
* DESCRIPTION :
*       * Reads temperature and humidity values from HTU21D sensor
*       * Writes the values to 2x16 character LCD
*       * Current dialect: Swedish-speaking Ostrobothnia
*
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <mcp23008.h>
#include <lcd.h>
#include <htu21d.h>

// HTU WiringPi defs
#define HTU_BASE        200
#define HTU_TEMP        (HTU_BASE + 0)
#define HTU_HUMID       (HTU_BASE + 1)

// LCD defs (2x16 characters, 4-bit mode)
#define LCD_ROWS        2
#define LCD_COLUMNS     16
#define LCD_CHARS       LCD_ROWS * LCD_COLUMNS
#define LCD_BITS        4

// LCD WiringPi defs
#define	AF_BASE	        100
#define AF_RS           (AF_BASE + 1)
#define AF_E            (AF_BASE + 2)
#define	AF_LIGHT        (AF_BASE + 7)
#define AF_DATAMASK     0x78
#define	AF_DB4	        (AF_BASE + 3)
#define	AF_DB5	        (AF_BASE + 4)
#define	AF_DB6	        (AF_BASE + 5)
#define	AF_DB7	        (AF_BASE + 6)


// Global lcd handle:
static int lcdHandle ;

// Sets the LCD back light on. NOTE: need to set the AF_LIGHT pin direction first
static void setBacklightOn ()
{
    digitalWrite (AF_LIGHT,   0x1) ;
}


int main (int argc, char *argv[])
{

    int temp;
    int humidity;
    int htu_status;

    // LCD character buffer
    char buf [LCD_CHARS] ;


    // Setup Wiringpi
    wiringPiSetupSys () ;


    // Initialize the HTU21D humidity/temperature sensor, exit if failed
    htu_status = htu21dSetup (HTU_BASE) ;
    
    if (htu_status == 0)
    {
        fprintf (stderr, "HTUInit failed\n") ;
        exit (EXIT_FAILURE) ;
    }


    // Initialize the MCP23008 I2C IO expander
    mcp23008Setup (AF_BASE, 0x20) ;


    // Set the LCD backlight pin to output
    pinMode (AF_LIGHT, OUTPUT) ;


    // Initialize the LCD (via MCP23008), exit if failed
    lcdHandle = lcdInit (LCD_ROWS, LCD_COLUMNS, LCD_BITS, AF_RS, AF_E, AF_DB4,AF_DB5,AF_DB6,AF_DB7, 0,0,0,0) ;
 
    if (lcdHandle < 0)
    {
        fprintf (stderr, "lcdInit failed\n") ;
        exit (EXIT_FAILURE) ;
    }


    // Turn on the backlight
    setBacklightOn();


    // Clear the LCD
    lcdClear(lcdHandle) ;


    // Infinite loop: read the temperature and humidity after specified interval
    for (;;) {
  

        // Read the temperature and humidity values
        temp = analogRead(HTU_TEMP);
        humidity = analogRead(HTU_HUMID);
        
        // If temp <= -999 -> CRC has failed -> don't update the value
        if (temp > -999) {

            // Compose the temperature line
            sprintf (buf, "Temperatyr..%02d%cC", temp/10, (char)223) ;

            // Set the LCD cursor to first column, first row
            lcdPosition (lcdHandle, 0, 0);

            // Send the temperature line to the LCD
            lcdPuts(lcdHandle, buf);
        
        }

        // If humidity <= -999 -> CRC has failed -> don't update the value
        if (humidity > -999) {
        
            // Compose the humidity line
            sprintf (buf, "Fuktiheit...%02d %%", humidity/10) ;

            // Set the LCD cursor to first column, second row
            lcdPosition (lcdHandle, 0, 1) ;

            // Send the humidity line to the LCD
            lcdPuts(lcdHandle, buf) ;

        }

        // Wait for a while before the next update
        delay(5000);

    }

    return 0 ;
}
