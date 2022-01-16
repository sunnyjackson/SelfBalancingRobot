

// Maximum Buffer Size for Writing-to & Reading-from UART
#define MAX_UARTBUFFER_SIZE     100

// Initialize UART Port
void UART_Init(void);

// Write to UART Port
void UART_Tx(char* data, uint8_t count);

// Print message to terminal
void UART_print(char* msg);
