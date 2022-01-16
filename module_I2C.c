#include <msp430.h>
#include <stdint.h>
#include "module_I2C.h"

//--------------------------------------------------------
//-- I2C Module Public Functions
void I2C_Init(void)
{
    //-- initialize USCI Module B0 for I2C
    UCB0CTL1 |= UCSWRST;       // put in module software reset mode, to enable module configuration
    UCB0CTL1 |= UCSSEL_2;      // select SMCLK (1 MHz) as I2C clock source
    UCB0BRW = 10;               // set clock pre-scalar to 10. SCLK = 1MHz / 10 = 100kHz
    UCB0CTL0 |= UCMODE_3;      // select I2C mode
    UCB0CTL0 |= UCMST;         // set as master

    //-- setup ports
    P3SEL |= BIT0;              // P3.0 = UCB0SDA
    P3SEL |= BIT1;              // P3.1 = UCB0SCL

    //-- take USCI Module B0 out of SW RST
    UCB0CTL1 &= ~UCSWRST;

    // initialize i2c state machine fields
    i2c.MasterMode = IDLE_MODE;
    i2c.RXByteCtr = 0;
    i2c.TXByteCtr = 0;
    i2c.RXIndex = 0;
    i2c.TXIndex = 0;
    i2c.RegAddr = 0;
}

void I2C_WriteByte(uint8_t dev_addr, uint8_t reg_addr, uint8_t reg_data)
{
    while(i2c.MasterMode != IDLE_MODE);

    // Initialize state machine
    i2c.MasterMode = TX_REG_ADDRESS_MODE;
    i2c.RegAddr = reg_addr;

    //Copy register data to TXBuffer
    i2c.TXBuffer[0] = reg_data;

    i2c.TXByteCtr = 1;
    i2c.RXByteCtr = 0;
    i2c.RXIndex = 0;
    i2c.TXIndex = 0;

    // Initialize slave address and interrupts
    UCB0I2CSA = dev_addr;
    UCB0IFG &= ~(UCTXIFG + UCRXIFG);         // Clear any pending interrupts
    UCB0IE &= ~UCRXIE;                       // Disable RX interrupt
    UCB0IE |= UCTXIE;                        // Enable TX interrupt

    UCB0CTL1 |= UCTR + UCTXSTT;              // I2C TX, start condition
    __enable_interrupt(); // I replace this line, because I don't understand LPM on the MSP430: __bis_SR_register(LPM0_bits + GIE);              // Enter LPM0 w/ interrupts

    while(i2c.MasterMode != IDLE_MODE); // wait for session to be complete

    return;
}

void I2C_WriteBuffer(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t count)
{
    while(i2c.MasterMode != IDLE_MODE);

    // Initialize state machine
    i2c.MasterMode = TX_REG_ADDRESS_MODE;
    i2c.RegAddr = reg_addr;

    // Copy register data to TXBuffer
    uint8_t copyIndex;
    for (copyIndex = 0; copyIndex < count; copyIndex++)
    {
        i2c.TXBuffer[copyIndex] = reg_data[copyIndex];
    }

    i2c.TXByteCtr = count;
    i2c.RXByteCtr = 0;
    i2c.RXIndex = 0;
    i2c.TXIndex = 0;

    // Initialize slave address and interrupts
    UCB0I2CSA = dev_addr;
    UCB0IFG &= ~(UCTXIFG + UCRXIFG);         // Clear any pending interrupts
    UCB0IE &= ~UCRXIE;                       // Disable RX interrupt
    UCB0IE |= UCTXIE;                        // Enable TX interrupt

    UCB0CTL1 |= UCTR + UCTXSTT;              // I2C TX, start condition
    __enable_interrupt(); // I replace this line, because I don't understand LPM on the MSP430: __bis_SR_register(LPM0_bits + GIE);              // Enter LPM0 w/ interrupts

    while(i2c.MasterMode != IDLE_MODE); // wait for session to be complete

    return;
}


void I2C_ReadBuffer(uint8_t dev_addr, uint8_t reg_addr, uint8_t count)
{
    while(i2c.MasterMode != IDLE_MODE);

    // Initialize state machine
    i2c.MasterMode = TX_REG_ADDRESS_MODE;
    i2c.RegAddr = reg_addr;
    i2c.RXByteCtr = count;
    i2c.TXByteCtr = 0;
    i2c.RXIndex = 0;
    i2c.TXIndex = 0;

    // Initialize slave address and interrupts
    UCB0I2CSA = dev_addr;
    UCB0IFG &= ~(UCTXIFG + UCRXIFG);       // Clear any pending interrupts
    UCB0IE &= ~UCRXIE;                     // Disable RX interrupt
    UCB0IE |= UCTXIE;                      // Enable TX interrupt

    UCB0CTL1 |= UCTR + UCTXSTT;            // I2C TX, start condition
    __enable_interrupt(); // I replace this line, because I don't understand LPM on the MSP430: __bis_SR_register(LPM0_bits + GIE);              // Enter LPM0 w/ interrupts

    while(i2c.MasterMode != IDLE_MODE); // wait for session to be complete

    return;
}

uint8_t I2C_ReadByte(uint8_t dev_addr, uint8_t reg_addr)
{
    while(i2c.MasterMode != IDLE_MODE);

    // Initialize state machine
    i2c.MasterMode = TX_REG_ADDRESS_MODE;
    i2c.RegAddr = reg_addr;
    i2c.RXByteCtr = 1;
    i2c.TXByteCtr = 0;
    i2c.RXIndex = 0;
    i2c.TXIndex = 0;

    // Initialize slave address and interrupts
    UCB0I2CSA = dev_addr;
    UCB0IFG &= ~(UCTXIFG + UCRXIFG);       // Clear any pending interrupts
    UCB0IE &= ~UCRXIE;                     // Disable RX interrupt
    UCB0IE |= UCTXIE;                      // Enable TX interrupt

    UCB0CTL1 |= UCTR + UCTXSTT;            // I2C TX, start condition
    __enable_interrupt();

    while(i2c.MasterMode != IDLE_MODE); // wait for session to be complete

    return i2c.RXBuffer[0];
}

//--------------------------------------------------------
//-- I2C ISR
#pragma vector=USCI_B0_VECTOR
__interrupt void USCI_B0_I2C_ISR(void)
{
    P1OUT ^= BIT0; // toggle P1.0 LED for troubleshooting

    uint8_t rx_val = 0;

    switch(__even_in_range(UCB0IV, 0xC))
    {
        case USCI_I2C_UCRXIFG: // Data has been received and is ready to be pulled from the I2C receive buffer
            rx_val = UCB0RXBUF;
            if (i2c.RXByteCtr)
            {
              i2c.RXBuffer[i2c.RXIndex++] = rx_val;
              i2c.RXByteCtr--;
            }

            if (i2c.RXByteCtr == 1)
            {
              UCB0CTL1 |= UCTXSTP;
            }
            else if (i2c.RXByteCtr == 0)
            {
              UCB0IE &= ~UCRXIE;
              i2c.MasterMode = IDLE_MODE;
              // I removed this line, because I don't understand LPM on the MSP430: __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
            }
            break;

        case USCI_I2C_UCTXIFG: // Slave has ACK'd, and is ready to receive
            switch (i2c.MasterMode)
                {
                  case TX_REG_ADDRESS_MODE:
                      UCB0TXBUF = i2c.RegAddr;
                      if (i2c.RXByteCtr)
                          i2c.MasterMode = SWITCH_TO_RX_MODE;
                      else
                          i2c.MasterMode = TX_DATA_MODE;
                      break;

                  case SWITCH_TO_RX_MODE:
                      UCB0IE |= UCRXIE;              // Enable RX interrupt
                      UCB0IE &= ~UCTXIE;             // Disable TX interrupt
                      UCB0CTL1 &= ~UCTR;            // Switch to receiver
                      i2c.MasterMode = RX_DATA_MODE;    // State state is to receive data
                      UCB0CTL1 |= UCTXSTT;          // Send repeated start (I'm surprised I need this? It seems like it might be to handle a use-case that I'm not using)
                      if (i2c.RXByteCtr == 1)
                      {
                          //Must send stop since this is the N-1 byte
                          while((UCB0CTL1 & UCTXSTT));
                          UCB0CTL1 |= UCTXSTP;      // Send stop condition
                      }
                      break;

                  case TX_DATA_MODE:
                      if (i2c.TXByteCtr)
                      {
                          UCB0TXBUF = i2c.TXBuffer[i2c.TXIndex++];
                          i2c.TXByteCtr--;
                      }
                      else
                      {
                          //Done with transmission
                          UCB0CTL1 |= UCTXSTP;     // Send stop condition
                          i2c.MasterMode = IDLE_MODE;
                          UCB0IE &= ~UCTXIE;                       // disable TX interrupt
                          // I removed this line, because I don't understand LPM on the MSP430: __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
                      }
                      break;

                  default:
                      __no_operation(); // Not sure why this no_op is here, but it seems harmless so I'm leaving it in
                      break;
                }
                break;

            default: break;
    }
}
