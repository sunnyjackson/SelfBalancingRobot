#include <msp430.h>
#include <stdint.h>
#include "module_Motor.h"


//--------------------------------------------------------
//-- Motor Module Public Functions
// Initialize Motor Interface
void Motor_Init(void)
{
    //-- set P6.3 and P6.4 for controlling motor direction
    P6OUT &= 0x00;
    P6DIR &= 0x00;
    P6DIR |= BIT3;
    P6DIR |= BIT4;
    P6OUT |= BIT4;
    P6OUT &= ~BIT3;

    //-- configure P8.2 as PWM output
    P8OUT &= 0x00;
    P8DIR &= 0x00;
    P8DIR |= BIT2;

    TA0CCR0 = 1000-1;                          // Set PWM Period
    TA0CCR1 = 500;                            // CCR1 PWM duty cycle
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
    if (duty > 100){return;}
    TA0CCR1 = duty*10; // CCR1 PWM duty cycle
}

// Initialize Motor Interface (1 = forward, -1 = backward, 0 = off)
void Motor_Direction(int8_t dir)
{
    switch (dir){
        case 1: // forward
            P6OUT |= BIT4;
            P6OUT &= ~BIT3;
            break;

        case -1: // backward
            P6OUT &= ~BIT4;
            P6OUT |= BIT3;
            break;

        default: // stop
            P6OUT &= ~BIT4;
            P6OUT &= ~BIT3;
            break;
    }
}

//--------------------------------------------------------
//-- PWM Timer Interrupts
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) // Period
{
    P8OUT |= BIT2;
    TA0CCTL0 &= ~CCIFG;
}
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void) // Duty Cycle
{
    P8OUT &= ~BIT2;
    TA0CCTL1 &= ~CCIFG;
}

