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
    if(MPU6050_SelfTest()){
        UART_print("\n\r--ERROR: MPU6050 Sensor SelfTest Failed");
    }else{
        UART_print("\n\rMPU6050 Sensor SelfTest Passed");
    }

    UART_print("\n");
    int16_t_xyz a;
    char msg[MAX_UARTBUFFER_SIZE];
	while(1) // use this loop to review accelerometer values, and verify that orienting the gravitation vector along each axis yields the expected readouts
	{
	    MPU6050_ReadAccel(&a);
	    snprintf(msg, MAX_UARTBUFFER_SIZE, "\rz: %i, y: %i, x:%i",a.z, a.y, a.x);
	    UART_print(msg);
	    UART_print("                                      "); // hacky method for clearing the terminal screen
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
