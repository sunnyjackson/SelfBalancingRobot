#include <msp430.h>				
#include <stdint.h>
#include "module_UART.h"
#include "module_I2C.h"
#include "module_MPU6050.h"

/*
  This example is based on:
      -

  This example shows:
      -

*/

//-- Function Prototypes
void LED_Init(void);

void main(void)
{
    //-- initializations
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer

	LED_Init();                     // Initialize on-board LED, for use as a monitor when troubleshooting

	UART_Init();                    // Initialize UART port, for reading out telemetry points in a desktop terminal
	uint8_t greet[] = "Hi!\n\r";
	UART_Tx(greet, sizeof(greet));
	__delay_cycles(10000);

    MPU6050_Init();                 // Initialize the MPU6050 port
    uint8_t status = 1;
	while(1)
	{
	    //UART_Tx(greet, sizeof(greet));
	    //__delay_cycles(1000);

	    //MPU6050_CheckI2C();
	    //UART_Tx(i2c.RXBuffer, 1);
	    //__delay_cycles(1000);

	    status = MPU6050_TestRegConfig() + 48; // add 48 to shift to get ASCII character
	    UART_Tx(&status, 1);
	    __delay_cycles(1000);
	    UART_Tx("\n\r", 2);
	    __delay_cycles(1000);
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
