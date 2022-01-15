#include <msp430.h>				
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "module_UART.h"
#include "module_I2C.h"
#include "module_MPU6050.h"

/*
  This example is based on:
      -

  This example shows:
      -

*/

//-- Globals
char uart_msg[MAX_UARTBUFFER_SIZE];           // General purpose buffer for storing strings to printout over the UART line

//-- Function Prototypes
void LED_Init(void);

void main(void)
{
    //-- initializations
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer

	LED_Init();                     // Initialize on-board LED, for use as a monitor when troubleshooting

	UART_Init();                    // Initialize UART port, for reading out telemetry points in a desktop terminal

	MPU6050_Init();                 // Initialize the MPU6050 port
	memset(uart_msg, 0, MAX_UARTBUFFER_SIZE);
	if(MPU6050_CheckI2C()){
        snprintf(uart_msg, sizeof(uart_msg), "\n\r--ERROR: I2C Bus Functional Test Failed");
        UART_Tx(uart_msg,strlen(uart_msg));
    }else{
        snprintf(uart_msg, sizeof(uart_msg), "\n\rI2C Bus Functional Test Passed");
        UART_Tx(uart_msg,strlen(uart_msg));
    }
    __delay_cycles(10000);
    memset(uart_msg, 0, strlen(uart_msg));

    if(MPU6050_TestRegConfig()){
        snprintf(uart_msg, sizeof(uart_msg), "\n\r--ERROR: MPU6050 Register Initialization Verification Failed");
        UART_Tx(uart_msg,strlen(uart_msg));
    }else{
        snprintf(uart_msg, sizeof(uart_msg), "\n\rMPU6050 Register Initialization Verification Passed");
        UART_Tx(uart_msg,strlen(uart_msg));
    }
    __delay_cycles(10000);
    memset(uart_msg, 0, strlen(uart_msg));


    int16_t a[3];
    int16_t g[3];
    UART_Tx("\n\r\n\r",2);
	while(1)
	{
	    MPU6050_ReadAccel(a);
	    MPU6050_ReadGyro(g);
	    snprintf(uart_msg, sizeof(uart_msg), "x: %i, y: %i, z:%i, x: %i, y: %i, z:%i \r",a[0], a[1], a[2], g[0], g[1], g[2]);
	    UART_Tx(uart_msg,strlen(uart_msg));
	    __delay_cycles(10000);
	    memset(uart_msg, 0, strlen(uart_msg));
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
