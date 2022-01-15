#include <msp430.h>
#include <stdint.h>
#include "module_UART.h"

//--------------------------------------------------------
//-- Declare UART State Machine Data Structures
typedef struct UARTSM{
    uint8_t TXBuffer[MAX_UARTBUFFER_SIZE]; // Buffer used to transmit data in the ISR
    uint8_t TXByteCtr;                  // Number of bytes left to transfer
    uint8_t TXIndex;                    // The index of the next byte to be transmitted out of TXBuffer
} UARTStateMachine;
UARTStateMachine uart;


//--------------------------------------------------------
//-- UART Module Public Functions
            // TODO: Consider accepting Baudrate argument
void UART_Init(void)
{
    //-- initialize USCI Module A1 for UART
        UCA1CTL1 |= UCSWRST;       // put in module software reset mode, to enable module configuration
        UCA1CTL1 |= UCSSEL_2;      // select SMCLK (1 MHz) as clock source
        UCA1BR0 = 9;                              // 1MHz 115200 (see User's Guide)
        UCA1BR1 = 0;                              // 1MHz 115200
        UCA1MCTL |= UCBRS_1 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0

        //-- setup ports
        P4SEL |= BIT5;              // P4.5 = UCA1RXD
        P4SEL |= BIT4;              // P4.4 = UCA1TXD

        //-- Set UCLISTEN bit in UCAxSTAT register, to pipe TxD into RxD. We'll use this to fire an interrupt whenever the RX buffer is full, indicating that the Tx Shift Register has been emptied
        UCA1STAT |= UCLISTEN; // Note that whenever we're doing this, I think it means we're only configured for simplex UART

        //-- take USCI Module A1 out of SW RST
        UCA1CTL1 &= ~UCSWRST;

        // initialize uart state machine fields
        uart.TXByteCtr = 0;
        uart.TXIndex = 0;
}

void UART_Tx(char* data, uint8_t count)
{
    //Initialize uart state machine
        // TODO: Consider rewriting this module so that it doesn't need to use CopyArray, and instead just uses the pointer it receives to step through the buffer (although this would require no modifications to that buffer during the transmission period...)
    uint8_t copyIndex;
    for (copyIndex = 0; copyIndex < count; copyIndex++)
    {
        uart.TXBuffer[copyIndex] = data[copyIndex];
    }
    uart.TXByteCtr = count;
    uart.TXIndex = 0;

    // Initialize interrupts
    UCA1IFG &= ~(UCTXIFG + UCRXIFG);         // Clear any pending interrupts
    UCA1IE |= UCRXIE;                        // Enable RX interrupt, which has TxD piped into it to indicate when the TxD shift register is empty

    UCA1TXBUF = uart.TXBuffer[uart.TXIndex++];
    uart.TXByteCtr--;
    __enable_interrupt();
}


//--------------------------------------------------------
//-- UART ISR
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_UART_ISR(void)
{
    P1OUT ^= BIT0; // toggle onboard LED, for debugging
    switch(__even_in_range(UCA1IV,4))
    {
        case 0:break;                             // Vector 0 - no interrupt
        case 2:                                   // Vector 2 - RXIFG
            if (uart.TXByteCtr)
            {
                UCA1TXBUF = uart.TXBuffer[uart.TXIndex++];
                uart.TXByteCtr--;
            }
            break;
        case 4:break;                             // Vector 4 - TXIFG
        default: break;
    }
}
