#include <msp430.h>
#include <stdint.h>
#include "module_Motor.h"


//--------------------------------------------------------
//-- Motor Module Public Functions
// Initialize Motor Interface
void Motor_Init(void)
{
    //-- set P6.3 and P6.4 for controlling left-motor direction
    P6OUT &= 0x00;
    P6DIR &= 0x00;
    P6DIR |= BIT4;
    P6DIR |= BIT3;
    P6OUT |= BIT4;
    P6OUT &= ~BIT3;
    //-- set P6.1 and P6.2 for controlling right-motor direction
    P6DIR |= BIT2;
    P6DIR |= BIT1;
    P6OUT |= BIT2;
    P6OUT &= ~BIT1;

    //-- configure P1.4 & P1.5 as PWM outputs for motor speed control
    P1DIR |= BIT5;
    P1SEL |= BIT5;
    P1DIR |= BIT4;
    P1SEL |= BIT4;

    TA0CCR0 = 2000-1;                          // Set PWM Period (2000 -> 500 Hz)
    TA0CCR3 = 10;                              // CCR3 PWM duty cycle
    TA0CCR4 = 10;                              // CCR4 PWM duty cycle
    TA0CCTL3 = OUTMOD_7;                      // CCR3 reset/set
    TA0CCTL4 = OUTMOD_7;                      // CCR4 reset/set
    TA0CTL |= TACLR; // Clear Timer A counter
    TA0CTL |= MC_1; // Count-up to CCR0
    TA0CTL |= TASSEL_2; // Select SMCLK (~1 MHz)

    TA0CCTL0 |= CCIE;    // CCR0 interrupt enable (period)
    TA0CCTL3 |= CCIE;    // CCR3 interrupt enable (duty cycle, left)
    TA0CCTL4 |= CCIE;    // CCR4 interrupt enable (duty cycle, right)
    __enable_interrupt();
}

// Update Motor Duty Cycle, using a value between 0-100
void Motor_SetDutyCycle(uint8_t duty)
{
    uint8_t s = 20; // scale factor, to convert 0-100 duty cycle to TA0CCR clock cycles
    if (duty >= 100){ // maximum duty cycle
        TA0CCR3 = 99*s;
        TA0CCR4 = 99*s;
    }
    else{
        TA0CCR3 = (duty + 13)*s; // + deadzone compensatio
        TA0CCR4 = (duty + 14)*s; // + deadzone compensatio
    }
}

// Initialize Motor Interface (1 = forward, -1 = backward, 0 = off)
void Motor_Direction(int8_t dir)
{
    switch (dir){
        case 1: // forward
            P6OUT |= BIT4;
            P6OUT &= ~BIT3;
            P6OUT |= BIT2;
            P6OUT &= ~BIT1;
            break;

        case -1: // backward
            P6OUT &= ~BIT4;
            P6OUT |= BIT3;
            P6OUT &= ~BIT2;
            P6OUT |= BIT1;
            break;

        default: // stop
            P6OUT &= ~BIT4;
            P6OUT &= ~BIT3;
            P6OUT &= ~BIT2;
            P6OUT &= ~BIT1;
            break;
    }
}

//--------------------------------------------------------
//-- PWM Timer Interrupts
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) // Period
{
    TA0CCTL0 &= ~CCIFG;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void) // Duty Cycle
{
    if (TA0CCTL4 & CCIFG == CCIFG){
        TA0CCTL4 &= ~CCIFG;
    }
    else if (TA0CCTL3 & CCIFG == CCIFG){
            TA0CCTL3 &= ~CCIFG;
    }
}
