#include <msp430.h>				
#include <stdint.h>
#include <stdio.h>
#include "module_Motor.h"
#include "module_UART.h"


/*
  This example shows a simple PWM routine that bounces the duty cycle between 100% & 0%
*/

//-- Function Prototypes
void LED_Init(void);

void main(void)
{
    //-- initializations
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	LED_Init();                     // Initialize on-board LED, for use as a monitor when troubleshooting
	UART_Init();                    // Initialize UART port, for reading out telemetry points in a desktop terminal
	Motor_Init();
    //-- main loop
	uint8_t duty = 0;
	uint8_t grow= 1;
	while(1)
	{
	    P1OUT ^= BIT0; // toggle LED to show aliveness
	    Motor_SetDutyCycle(duty);
	    if (grow){
	        duty = (duty+1);
	        if(duty >= 100){
	            grow = 0;
	        }
	    }
	   else{
	       duty = (duty-1);
               if(duty == 0){
                   grow = 1;
               }
	    }
	    __delay_cycles(50000);
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
