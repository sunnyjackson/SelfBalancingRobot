
// Maximum Buffer Size for Writing-to & Reading-from I2C
#define MAX_I2CBUFFER_SIZE     20

// Initialize I2C Port
void I2C_Init(void);

// Write to I2C Port
void I2C_WriteReg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t count);

// Read from I2C Port
void I2C_ReadReg(uint8_t dev_addr, uint8_t reg_addr, uint8_t count);
