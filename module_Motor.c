#include <msp430.h>
#include <stdint.h>
#include "module_Motor.h"


//--------------------------------------------------------
//-- Motor Module Public Functions
// Initialize Motor Interface
void Motor_Init(void)
{
    //P1SEL |= BIT2+BIT3;                     // P1.2 and P1.3 options select
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

