// Maximum Buffer Size for Writing-to & Reading-from UART
#define MAX_UARTBUFFER_SIZE     20

// Initialize UART Port
void UART_Init(void);

// Write to UART Port
void UART_Tx(uint8_t* data, uint8_t count);

// This module does not yet support UART_Rx
