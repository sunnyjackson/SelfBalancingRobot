#include <msp430.h>
#include <stdint.h>
#include <math.h>
#include "mpu6050.h"
#include "module_MPU6050.h"
#include "module_I2C.h"


//--------------------------------------------------------
//-- Private Global variables
int16_t_xyz a_offset, g_offset;


//--------------------------------------------------------
//-- MPU6050 Module Public Functions
void MPU6050_Init(void)
{
    // Initialize I2C connection to the MPU6050
    I2C_Init();

    // Reset the MPU6050 device
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, DEVICE_RESET);
    while(I2C_ReadByte(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1) != 0x40); // Wait for the the device reset to complete

    // Wake up the MPU6050
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, 0x00);

    // Set sample rate
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_SMPLRT_DIV, 0x00);

    // Configure low pass filter
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_CONFIG, EXT_SYNC_SET_INPUT_DISABLE + DLPF_CFG_BAND_WIDTH_10HZ);

    // Configure accelerometer range
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, AFS_SEL_SCALE_2G);

    // Congfigure gyroscope range
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, PS_SEL_SCALE_250);

    // Select clock source
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, CLKSEL_3);

    // Initialize offsets
    a_offset.x = 0;
    a_offset.y = 0;
    a_offset.z = 0;
    g_offset.x = 0;
    g_offset.y = 0;
    g_offset.z = 0;
}


/*--------------------------------------------------------------------------------
Function    : MPU6050_CheckI2C
Purpose     : Check I2C communication
Parameters  : None
Return      : 0 if pass and 1 if fail
-------------------------------------------------------------------------------*/
uint8_t MPU6050_CheckI2C(void)
{
    if (I2C_ReadByte(MPU6050_ADDRESS, MPU6050_WHO_AM_I) == 0x68){
        return 0;
    }else{
        return 1;
    }
}


/*--------------------------------------------------------------------------------
Function    : MPU6050_TestRegConfig
Purpose     : Check the config of key registers:
Parameters  : None
Return      : 0 if pass and 1 if fail
-------------------------------------------------------------------------------*/
uint8_t MPU6050_TestRegConfig(void)
{
    if (I2C_ReadByte(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1) != CLKSEL_3)
        {return 1;} // error

    if (I2C_ReadByte(MPU6050_ADDRESS, MPU6050_SMPLRT_DIV) != 0x00)
        {return 1;} // error

    if (I2C_ReadByte(MPU6050_ADDRESS, MPU6050_CONFIG) != EXT_SYNC_SET_INPUT_DISABLE + DLPF_CFG_BAND_WIDTH_10HZ)
        {return 1;} // error

    if (I2C_ReadByte(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG) != PS_SEL_SCALE_250)
        {return 1;} // error

    if (I2C_ReadByte(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG) != AFS_SEL_SCALE_2G)
        {return 1;} // error

    return 0; // success
}


/*--------------------------------------------------------------------------------
Function    : MPU6050_ReadAccel
Purpose     : Get raw value x, y, z of accel
Parameters  : pointer to a struct to store acc data
Return      : NULL
--------------------------------------------------------------------------------*/
void MPU6050_ReadAccel(int16_t_xyz* a)
{
    I2C_ReadBuffer(MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_H, 6);
    a->x = (i2c.RXBuffer[0]<<8 | i2c.RXBuffer[1]) + a_offset.x;// / 16384;
    a->y = (i2c.RXBuffer[2]<<8 | i2c.RXBuffer[3]) + a_offset.y;// / 16384;
    a->z = (i2c.RXBuffer[4]<<8 | i2c.RXBuffer[5]) + a_offset.z;// / 16384;
}


/*--------------------------------------------------------------------------------
Function    : MPU6050_GetGyroValueRaw
Purpose     : Get raw value x, y, z of Gyro
Parameters  : pointer to struct to store gyro data
Return      : NULL
--------------------------------------------------------------------------------*/
void MPU6050_ReadGyro(int16_t_xyz* g)
{
    I2C_ReadBuffer(MPU6050_ADDRESS, MPU6050_GYRO_XOUT_H, 6);
    g->x = (i2c.RXBuffer[0]<<8 | i2c.RXBuffer[1]) + g_offset.x;// / 131;
    g->y = (i2c.RXBuffer[2]<<8 | i2c.RXBuffer[3]) + g_offset.y;// / 131;
    g->z = (i2c.RXBuffer[4]<<8 | i2c.RXBuffer[5]) + g_offset.z;// / 131;
}


void GetMeanAccel(int16_t_xyz* a_mean, uint16_t num_samples)
{
    // Gather an average reading, with self-test OFF
    int16_t_xyz a;
    uint16_t i;
    a_mean->x = 0;
    a_mean->y = 0;
    a_mean->z = 0;
    for (i = 0; i < num_samples; i++)
    {
        MPU6050_ReadAccel(&a);
        a_mean->x += a.x/num_samples;
        a_mean->y += a.y/num_samples;
        a_mean->z += a.z/num_samples;
    }
}

void GetMeanGyro(int16_t_xyz* g_mean, uint16_t num_samples)
{
    // Gather an average reading, with self-test OFF
    int16_t_xyz g;
    uint16_t i;
    g_mean->x = 0;
    g_mean->y = 0;
    g_mean->z = 0;
    for (i = 0; i < num_samples; i++)
    {
        MPU6050_ReadGyro(&g);
        g_mean->x += g.x/num_samples;
        g_mean->y += g.y/num_samples;
        g_mean->z += g.z/num_samples;
    }
}

/*--------------------------------------------------------------------------------
Function    : MPU6050_SelfTest
Purpose     : Perform self-test on accel & gyro (note: IMU must be stationary during this test)
Parameters  : none
Return      : 0 if pass self-test, 1 if fail self-test, 2 if unable to perform self-test
--------------------------------------------------------------------------------*/
uint8_t MPU6050_SelfTest(void)
{
// Define self-test register configurations
#define GYRO_SELFTEST_OFF  0x00 // gyro should be configured with range = +/- 250 dps when performing a self-test
#define GYRO_SELFTEST_ON  0xE0
#define ACCEL_SELFTEST_OFF  0x10 // accel should be configured with range = +/- 8 g when performing a self-test
#define ACCEL_SELFTEST_ON  0xF0

    // Verify MPU6050 is properly configured
    if(MPU6050_TestRegConfig()) {return 2;} // user must ensure MPU6050 registers are configured prior to performing a self-test

    // Turn OFF self-test mode
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, GYRO_SELFTEST_OFF);
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, ACCEL_SELFTEST_OFF);
    // Gather an average reading, with self-test OFF
    int16_t_xyz a_avg_off, g_avg_off;
    uint16_t num_samples = 300;
    GetMeanAccel(&a_avg_off, num_samples);
    GetMeanGyro(&g_avg_off, num_samples);

    // Turn ON self-test mode
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, GYRO_SELFTEST_ON);
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, ACCEL_SELFTEST_ON);
    // Gather an average reading, with self-test ON
    int16_t_xyz a_avg_on, g_avg_on;
    GetMeanAccel(&a_avg_on, num_samples);
    GetMeanGyro(&g_avg_on, num_samples);

    // Turn OFF self-test mode, returning register configurations to their initial states
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, PS_SEL_SCALE_250);
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, AFS_SEL_SCALE_2G);

    // Calculate Factory trim values from onboard the MPU6050 (stored as 5-bit unsigned integers)
    I2C_ReadBuffer(MPU6050_ADDRESS, 0x0D, 4);
    uint8_t XA_TEST = ((i2c.RXBuffer[0] & 0xE0)>>3) | ((i2c.RXBuffer[3] & 0x30)>>4);
    uint8_t YA_TEST = ((i2c.RXBuffer[1] & 0xE0)>>3) | ((i2c.RXBuffer[3] & 0x0C)>>2);
    uint8_t ZA_TEST = ((i2c.RXBuffer[2] & 0xE0)>>3) | ((i2c.RXBuffer[3] & 0x03)>>0);
    uint8_t XG_TEST = i2c.RXBuffer[0] & 0x1F;
    uint8_t YG_TEST = i2c.RXBuffer[1] & 0x1F;
    uint8_t ZG_TEST = i2c.RXBuffer[2] & 0x1F;
    float a_FT[3], g_FT[3];
    a_FT[0] = 4096.0*0.34*(pow( (0.92/0.34) , (((float)XA_TEST - 1.0)/30.0))); // FT[Xa] factory trim calculation
    a_FT[1] = 4096.0*0.34*(pow( (0.92/0.34) , (((float)YA_TEST - 1.0)/30.0))); // FT[Ya] factory trim calculation
    a_FT[2] = 4096.0*0.34*(pow( (0.92/0.34) , (((float)ZA_TEST - 1.0)/30.0))); // FT[Za] factory trim calculation
    g_FT[0] =  25.0*131.0*(pow( 1.046 , ((float)XG_TEST - 1.0) ));             // FT[Xg] factory trim calculation
    g_FT[1] = -25.0*131.0*(pow( 1.046 , ((float)YG_TEST - 1.0) ));             // FT[Yg] factory trim calculation
    g_FT[2] =  25.0*131.0*(pow( 1.046 , ((float)ZG_TEST - 1.0) ));             // FT[Zg] factory trim calculation

    // Calculate Change from Factory Trim of the Self-Test Response
    float delta[6];
    delta[0] = 100*((float)(a_avg_on.x - a_avg_off.x) - a_FT[0]) / a_FT[0];
    delta[1] = 100*((float)(a_avg_on.y - a_avg_off.y) - a_FT[1]) / a_FT[1];
    delta[2] = 100*((float)(a_avg_on.z - a_avg_off.z) - a_FT[2]) / a_FT[2];
    delta[3] = 100*((float)(g_avg_on.x - g_avg_off.x) - g_FT[0]) / g_FT[0];
    delta[4] = 100*((float)(g_avg_on.y - g_avg_off.y) - g_FT[1]) / g_FT[1];
    delta[5] = 100*((float)(g_avg_on.z - g_avg_off.z) - g_FT[2]) / g_FT[2];

    // Determine if Self Test Response is within +/- 14% tolerance from Factory Trim
    int i;
    for (i = 0; i < 6; i++){
        if(abs(delta[i]) > 14){return 1;} // out of tolerance
    }
    return 0; // all values within tolerance. self-test passed!
}


/*--------------------------------------------------------------------------------
Function    : MPU6050_Calibrate
Purpose     : Calibrate offsets for accelerometer and gyroscope
Parameters  : none
Return      : none
--------------------------------------------------------------------------------*/
void MPU6050_Calibrate(void)
{
    // Define convergence tolerances
    uint8_t a_tol = 8;
    uint8_t g_tol = 1;

    // Get mean
    uint16_t num_samples = 300;
    int16_t_xyz a_mean, g_mean;
    GetMeanAccel(&a_mean, num_samples);
    GetMeanGyro(&g_mean, num_samples);

    // Initialize offsets
    a_offset.z = -a_mean.z/8;
    a_offset.y = -a_mean.y/8;
    a_offset.x = (16384-a_mean.x)/8; // When connected to my breadboard, gravity points along the +x vector
    g_offset.x = -g_mean.x/4;
    g_offset.y = -g_mean.y/4;
    g_offset.z = -g_mean.z/4;

    // Iterate on offsets until we converge within tolerances defined above
    while (1){
      P1OUT ^= BIT0; // toggle P1.0 LED for troubleshooting
      int ready=0;

      // Get the mean values from the sensor
      GetMeanAccel(&a_mean, num_samples);
      GetMeanGyro(&g_mean, num_samples);

      if (abs(a_mean.z) <= a_tol) ready++;
      else a_offset.z += -a_mean.z/a_tol;

      if (abs(a_mean.y) <= a_tol) ready++;
      else a_offset.y += -a_mean.y/a_tol;

      if (abs(16384-a_mean.x) <= a_tol) ready++;
      else a_offset.x += (16384-a_mean.x)/a_tol;

      if (abs(g_mean.x) <= g_tol) ready++;
      else g_offset.x += -g_mean.x/(g_tol+1);

      if (abs(g_mean.y) <= g_tol) ready++;
      else g_offset.y += -g_mean.y/(g_tol+1);

      if (abs(g_mean.z) <= g_tol) ready++;
      else g_offset.z += -g_mean.z/(g_tol+1);

      if (ready==6) break;
    }
}

void MPU6050_SetCalibration(void)
{
    a_offset.x = -580;
    a_offset.y = 430;
    a_offset.z = 3800;
    g_offset.x = 395;
    g_offset.y = 102;
    g_offset.z = 177;
}

/*--------------------------------------------------------------------------------
Function    : MPU6050_ReadAngle
Purpose     : Return an angle estimate, by combining accelerometer and gyroscope data
Parameters  : none
Return      : angle estimate
--------------------------------------------------------------------------------*/
float MPU6050_ReadAngle(void)
{
    static float theta = 0;
    float dt = 0.07; // integration timestep

    // Read in values
    int16_t_xyz g, a;
    MPU6050_ReadGyro(&g);
    MPU6050_ReadAccel(&a);

    // toggle P8.2, signaling the beginning of the float operation
    P8OUT ^= BIT2;

    float theta_a = atan2( (float) a.y/16384 , (float) a.x/16384) * 180/3.14159265;
    float theta_g = (float)-g.z/131*dt + theta;
    theta = 0.98*(theta_g) + 0.02*(theta_a);

    P8OUT ^= BIT2;

    return theta;
}
