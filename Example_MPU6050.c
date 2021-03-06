#include <msp430.h>				
#include <stdint.h>
#include <stdio.h>
#include "module_MPU6050.h"
#include "module_UART.h"


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
	MPU6050_Init();                 // Initialize the MPU6050 port
	if(MPU6050_CheckI2C()){
	    UART_print("\n\r--ERROR: I2C Bus Functional Test Failed");
    }else{
        UART_print("\n\rI2C Bus Functional Test Passed");
    }
    if(MPU6050_TestRegConfig()){
        UART_print("\n\r--ERROR: MPU6050 Register Initialization Verification Failed");
    }else{
        UART_print("\n\rMPU6050 Register Initialization Verification Passed");
    }
    MPU6050_SetCalibration();

    //-- main loop
    UART_print("\n");
    int16_t theta;
    char msg[MAX_UARTBUFFER_SIZE];
	while(1)
	{
	    theta = MPU6050_ReadAngle()/131;
	    snprintf(msg, MAX_UARTBUFFER_SIZE, "\rtheta: %i",theta);
	    UART_print(msg);
	    UART_print("               "); // hacky method for clearing the terminal screen
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

    //-- configure P8.2 GPIO for use as a monitor on a logic analyzer
    P8OUT &= 0x00;
    P8DIR &= 0x00;
    P8DIR |= BIT2;
}
