#include <msp430.h>				

/*
  This example is taken from: https://www.embeddedrelated.com/showarticle/182.php

  It shows:
      - Blinking the onboard LED at P4.7, based on onboard switch inputs from P1.1
      - Blinking the onboard LED at P1.0, based on interrupts from TimerA0

 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer

	// Configure 16-bit timer
	TA0CCTL0 = CCIE; // CCR0 interrupt enabled
	TA0CTL = TASSEL_2 | MC_1 | ID_3; // Use SMCLK source, count-up, divisor = 8
	TA0CCR0 = 10000; // (clock_freq/divisor)/(CCR0 Compare Register) = 12.5 Hz

	// LED Output Config
	P1OUT &= 0x00; // Shut down everything
	P1DIR &= 0x00;
	P1DIR |= BIT0; // configure P1.0 as output
	P4OUT &= 0x00;
	P4DIR &= 0x00;
	P4DIR |= BIT7; // configure P4.7 pin as output

	// Switch Input Config
	P2DIR &= ~BIT1; // Set P2.1 to input
	P2REN |= BIT1; // Enable PUR on P2.1 (default: GND)
	P2OUT |= BIT1; // Set PUR to active (+3.3V) mode

	// Switch Interrupt Config
	P2IE |= BIT1;                   // P2.1 interrupt enabled
	P2IES |= BIT1; // P2.1 Interrupt Edge Select = Hi->Lo edge
	P2IFG &= ~BIT1;                 // P2.1 interrupt flag cleared

	__bis_SR_register(CPUOFF + GIE); // Enable all interrupts, and enter LPM0
	while(1){}
}


// Timer A0 Interrupt ISR
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0(void){
    P1OUT ^= BIT0; // Toggle P1.0
}


// Port 2 Interrupt ISR
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void){
    P4OUT ^= BIT7; // Toggle P4.7
    P2IFG &= ~BIT1; // P2.1 interrupt flag cleared
}
