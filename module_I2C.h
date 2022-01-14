

//--------------------------------------------------------
//-- Module Settings
// Maximum Buffer Size for Writing-to & Reading-from I2C
#define MAX_I2CBUFFER_SIZE     20


//--------------------------------------------------------
//-- Public Functions
// Initialize I2C Port
void I2C_Init(void);

// Write 1-byte to I2C Port
void I2C_WriteByte(uint8_t dev_addr, uint8_t reg_addr, uint8_t reg_data);

// Write buffer to I2C Port
void I2C_WriteBuffer(uint8_t dev_addr, uint8_t reg_addr, uint8_t* reg_data, uint8_t count);

// Read from I2C Port
void I2C_ReadReg(uint8_t dev_addr, uint8_t reg_addr, uint8_t count);



//--------------------------------------------------------
//-- Declare I2C State Machine Data Structures
// TODO Rewrite the I2C module so that these data structures are private, and the Buffers to exchange data with the application program are just pointers provided by the calling function
typedef enum I2C_ModeEnum{
    IDLE_MODE,
    TX_REG_ADDRESS_MODE,
    TX_DATA_MODE,
    RX_DATA_MODE,
    SWITCH_TO_RX_MODE
} I2C_Mode;

typedef struct I2CSM{
    I2C_Mode MasterMode;                // Current State of I2CStateMachine
    uint8_t RegAddr;                    // Slave Register Address to read-from/write-to
    uint8_t RXBuffer[MAX_I2CBUFFER_SIZE];  // Buffer used to receive data in the ISR
    uint8_t TXBuffer[MAX_I2CBUFFER_SIZE];  // Buffer used to transmit data in the ISR
    uint8_t RXByteCtr;                  // Number of bytes left to receive
    uint8_t TXByteCtr;                  // Number of bytes left to transfer
    uint8_t RXIndex;                    // The index of the next byte to be received in RXBuffer
    uint8_t TXIndex;                    // The index of the next byte to be transmitted in TXBuffer
}I2CStateMachine;
I2CStateMachine i2c;
