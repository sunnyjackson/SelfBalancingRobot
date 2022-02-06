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

    //-- configure P8.2 as PWM output for left-motor
    P8OUT &= 0x00;
    P8DIR &= 0x00;
    P8DIR |= BIT2;
    //-- configure P8.1 as PWM output for right-motor
    P8DIR |= BIT1;

    TA0CCR0 = 2000-1;                          // Set PWM Period (2000 -> 500 Hz)
    TA0CCR1 = 10;                              // CCR1 PWM duty cycle
    TA0CCTL1 = OUTMOD_7;                      // CCR1 reset/set
    TA0CTL |= TACLR; // Clear Timer A counter
    TA0CTL |= MC_1; // Count-up to CCR0
    TA0CTL |= TASSEL_2; // Select SMCLK (~1 MHz)

    TA0CCTL0 |= CCIE;    // CCR0 interrupt enable (period)
    TA0CCTL1 |= CCIE;    // CCR1 interrupt enable (duty cycle)
    __enable_interrupt();
}

// Update Motor Duty Cycle, using a value between 0-100
void Motor_SetDutyCycle(uint8_t duty)
{
    uint8_t s = 20; // scale factor, to convert 0-100 duty cycle to TA0CCR clock cycles
    if (duty >= 100){ // maximum duty cycle
        TA0CCR1 = 99*s;
    }
    else if (duty < 25){ // threshold minimum duty cycle (25% threshold is good for a 500 Hz PWM frequency, with my motor)
        TA0CCR1 = 1; // off
    }
    else{
        TA0CCR1 = duty*s;
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
    P8OUT |= (BIT2 & BIT1);
    TA0CCTL0 &= ~CCIFG;
}
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void) // Duty Cycle
{
    P8OUT &= ~(BIT2 & BIT1);
    TA0CCTL1 &= ~CCIFG;
}

