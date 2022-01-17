//--------------------------------------------------------
//-- Public Struct, for organizing (x,y,z) parameters
typedef struct XYZ{
    int16_t x;
    int16_t y;
    int16_t z;
} int16_t_xyz;

//--------------------------------------------------------
//-- Public Functions
// Initialize MPU6050, using pre-defined configuration settings
void MPU6050_Init(void);

// Verify I2C bus functionality, by reading out MPU6050 slave device address
uint8_t MPU6050_CheckI2C(void);

// Verify successful register configurations after MPU6050_Init()
uint8_t MPU6050_TestRegConfig(void);

// Read a single set of values from accelerometer
void MPU6050_ReadAccel(int16_t_xyz* a);

// Read a single set of values from gyroscope
void MPU6050_ReadGyro(int16_t_xyz* g);

// Read Angle Estimate from sensor (using a complementary filter to combine gyro and accelerometer estimate)
float MPU6050_ReadAngle(void);

// Perform a Self Test
uint8_t MPU6050_SelfTest(void);

// Calibrate Sensors
void MPU6050_Calibrate(void);

// Manually set sensor calibration (to avoid lengthy calibration)
void MPU6050_SetCalibration(void);
