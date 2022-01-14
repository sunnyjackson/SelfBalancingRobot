#include <msp430.h>				
#include <stdint.h>
#include "module_I2C.h"

/*
  This example is based on:
      - Brock Lamere's Youtube Videos describing I2C on the MSP430: https://www.youtube.com/watch?v=p_YOsh7BSDE&list=PL643xA3Ie_EuHoNV7AgvJXq-z1hrE8vsm&index=82
      - The I2C module is a tailored version of example code from TI:  MSP430F552x Demo - USCI_B0, I2C Master multiple byte TX/RX

  This example shows:
      - Use of an I2C peripheral on the MSP430, called USCI Module B0

 */

//-- Function Prototypes
void LED_Init(void);

void main(void)
{
    //-- initializations
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	LED_Init();                     // Initialize on-board LED, for use as a monitor when troubleshooting
	I2C_Init();                     // Initialize an I2C port
	__enable_interrupt();

	int i;
	uint8_t slaveAddr = 0x68;
	uint8_t data[] = {10, 11, 12, 13, 14, 15};
	while(1)
	{
	    I2C_WriteBuffer(slaveAddr, 0x43, data, 6);
	    for(i=0; i<100; i++);   // delay
	    I2C_ReadReg(slaveAddr, 0x43, 6);
	    for(i=0; i<100; i++);   // delay
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
