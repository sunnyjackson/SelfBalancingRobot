#include <msp430.h>				
#include <stdint.h>
#include "module_UART.h"

/*
  This example is based on:
      - Brock Lamere's Youtube Videos describing UART on the MSP430: https://www.youtube.com/watch?v=VBRUyLcqXV4&list=PL643xA3Ie_EuHoNV7AgvJXq-z1hrE8vsm&index=91

  This example shows:
      - Using the UART line to repeatedly send a string via the UART/USB backhaul present on my MSP430 Launchpad, which hits a COM port on my PC and is viewable using the built-in terminal in CCS
        - The onboard LED is toggled each time a character is transmitted
 */

//-- Function Prototypes
void LED_Init(void);

void main(void)
{
    //-- initializations
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	LED_Init();                     // Initialize on-board LED, for use as a monitor when troubleshooting
	UART_Init();                    // Initialize an I2C port

	int i;
	uint8_t data[] = "Hello world!\n\r";
	while(1)
	{
	    UART_Tx(data, sizeof(data));
	    for(i=0; i<10000; i++);   // delay
	}
}


//--------------------------------------------------------
//-- Initialization Routines
void LED_Init(void)
{
    //-- configure P1.0 LED for troubleshooting
    P1OUT &= 0x00;
    P1DIR &= 0x00;
    P1DIR |= BIT0;
}
