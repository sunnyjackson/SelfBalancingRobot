
// Initialize MPU6050, using pre-defined configuration settings
void MPU6050_Init(void);

// Verify I2C bus functionality, by reading out MPU6050 slave device address
uint8_t MPU6050_CheckI2C(void);

// Verify successful register configurations after MPU6050_Init()
uint8_t MPU6050_TestRegConfig(void);

// Read all contents from MPU6050 FIFO


// Read some contents from MPU 6050 FIFO (maybe the most recent 10)
