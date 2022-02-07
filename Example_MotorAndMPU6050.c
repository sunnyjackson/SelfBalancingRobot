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
int16_t PID(int16_t e);

void main(void)
{
    //-- initializations
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	LED_Init();                     // Initialize on-board LED, for use as a monitor when troubleshooting
    UART_Init();                    // Initialize UART port, for reading out telemetry points in a desktop terminal
	Motor_Init();                   // Initialize GPIO pins for direction control and PWM motor speed control
    MPU6050_Init();                 // Initialize pins for communication with the MPU6050
    MPU6050_SetCalibration();

    //-- main loop
    int16_t angle, duty;
    int8_t fwd = 0; // 0 or 1
    char msg[MAX_UARTBUFFER_SIZE];
    while(1)
    {
        P1OUT ^= BIT0; // toggle LED to show aliveness
        P1OUT ^= BIT2;

        // Read in angle (deg, scaled by 131)
        angle = MPU6050_ReadAngle();

        // Set direction based on angle sign, if direction changed
        if (angle >= 0 && fwd == 1){
            Motor_Direction(-1);
            fwd = 0;
        }else if (angle < 0 && fwd == 0){
            Motor_Direction(1);
            fwd = 1;
        }

        //snprintf(msg, MAX_UARTBUFFER_SIZE, "\rtheta: %i",angle/131);
        //UART_print(msg);
        //UART_print("               "); // hacky method for clearing the terminal screen

        // Calculate duty cycle, using PID loop
        duty = PID(abs(angle));

        // Set duty cycle based on angle
        Motor_SetDutyCycle(duty);
    }
}


//--------------------------------------------------------
//-- Helper Functions
void LED_Init(void)
{
    //-- configure P1.0 LED for troubleshooting
    P1OUT &= 0x00;
    P1DIR &= 0x00;
    P1DIR |= BIT0;
    //-- configure P1.2 GPIO for use as a monitor
    P1DIR |= BIT2;
}


int16_t PID(int16_t e){
    //-- PID control loop parameters
    int16_t K_p = 1;
    int16_t K_i = 0;
    int16_t K_d = 1;
    static int16_t e_i = 0; // error integral
    static int16_t e0 = 0; // previous error (used in calculating derivative)
    int16_t dt = 1; // timestep

    //-- Calculate Derivative & Integral
    e_i += K_i*dt*e;

    int16_t control = (K_p * e*3/7 + e_i + K_d * (e-e0)/7)/131; // divide by 131, scale factor
    e0 = e;

    //-- Control thresholds
    if (control < 0 || abs(e) > 45*131) return 0;
    if (control > 100) return 100;
    return control;
}
