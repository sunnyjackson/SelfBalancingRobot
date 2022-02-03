#include <msp430.h>				
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "module_Motor.h"
#include "module_UART.h"
#include "module_MPU6050.h"


/*
  This example combines the MPU6050 and Motor modules, using the angle estimate from the MPU6050 to control the speed of the motor
*/

//-- Function Prototypes
void LED_Init(void);

void main(void)
{
    //-- initializations
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	LED_Init();                     // Initialize on-board LED, for use as a monitor when troubleshooting
	UART_Init();                    // Initialize UART port, for reading out telemetry points in a desktop terminal
	Motor_Init();                   // Initialize GPIO pins for direction control and PWM motor speed control
    MPU6050_Init();                 // Initialize pins for communication with the MPU6050
    //-- main loop
    int16_t angle = 0;
    char msg[MAX_UARTBUFFER_SIZE];
    while(1)
    {
        P1OUT ^= BIT0; // toggle LED to show aliveness

        // Read in angle
        angle = MPU6050_ReadAngle()/131;

        // Set duty cycle based on angle
        Motor_SetDutyCycle(abs(angle)*100/90);

        // Set direction based on angle sign
        if (angle >= 0){
            Motor_Direction(1);
        }else{
            Motor_Direction(-1);
        }
        snprintf(msg, MAX_UARTBUFFER_SIZE, "\rtheta: %i",angle);
        UART_print(msg);
        UART_print("               "); // hacky method for clearing the terminal screen

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
