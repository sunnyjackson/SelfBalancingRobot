#include <msp430.h>
#include <stdint.h>
#include <math.h>
#include "mpu6050.h"
#include "module_MPU6050.h"
#include "module_I2C.h"


//--------------------------------------------------------
//-- I2C Module Public Functions
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

    // TODO Perform self-test? (see accel_config and gyro_config registers)
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
    a->x = (i2c.RXBuffer[0]<<8 | i2c.RXBuffer[1]);// / 16384;
    a->y = (i2c.RXBuffer[2]<<8 | i2c.RXBuffer[3]);// / 16384;
    a->z = (i2c.RXBuffer[4]<<8 | i2c.RXBuffer[5]);// / 16384;
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
    g->x = (i2c.RXBuffer[0]<<8 | i2c.RXBuffer[1]);// / 131;
    g->y = (i2c.RXBuffer[2]<<8 | i2c.RXBuffer[3]);// / 131;
    g->z = (i2c.RXBuffer[4]<<8 | i2c.RXBuffer[5]);// / 131;
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
    int16_t_xyz a, g;
    int16_t a_avg_off[3] = {0, 0, 0};
    int16_t g_avg_off[3] = {0, 0, 0};
    uint8_t num_samples = 100;
    uint8_t i;
    for (i = 0; i < num_samples; i++)
    {
        MPU6050_ReadAccel(&a);
        a_avg_off[0] += a.x/num_samples;
        a_avg_off[1] += a.y/num_samples;
        a_avg_off[2] += a.z/num_samples;
        MPU6050_ReadGyro(&g);
        g_avg_off[0] += g.x/num_samples;
        g_avg_off[1] += g.y/num_samples;
        g_avg_off[2] += g.z/num_samples;
    }

    // Turn ON self-test mode
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, GYRO_SELFTEST_ON);
    I2C_WriteByte(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, ACCEL_SELFTEST_ON);
    // Gather an average reading, with self-test ON
    int16_t a_avg_on[3] = {0, 0, 0};
    int16_t g_avg_on[3] = {0, 0, 0};
    for (i = 0; i < num_samples; i++)
    {
        MPU6050_ReadAccel(&a);
        a_avg_on[0] += a.x/num_samples;
        a_avg_on[1] += a.y/num_samples;
        a_avg_on[2] += a.z/num_samples;
        MPU6050_ReadGyro(&g);
        g_avg_on[0] += g.x/num_samples;
        g_avg_on[1] += g.y/num_samples;
        g_avg_on[2] += g.z/num_samples;
    }

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
    delta[0] = 100*((float)(a_avg_on[0] - a_avg_off[0]) - a_FT[0]) / a_FT[0];
    delta[1] = 100*((float)(a_avg_on[1] - a_avg_off[1]) - a_FT[1]) / a_FT[1];
    delta[2] = 100*((float)(a_avg_on[2] - a_avg_off[2]) - a_FT[2]) / a_FT[2];
    delta[3] = 100*((float)(g_avg_on[0] - g_avg_off[0]) - g_FT[0]) / g_FT[0];
    delta[4] = 100*((float)(g_avg_on[1] - g_avg_off[1]) - g_FT[1]) / g_FT[1];
    delta[5] = 100*((float)(g_avg_on[2] - g_avg_off[2]) - g_FT[2]) / g_FT[2];

    // Determine if Self Test Response is within +/- 14% tolerance from Factory Trim
    for (i = 0; i < 6; i++){
        if(abs(delta[i]) > 14){return 1;} // out of tolerance
    }
    return 0; // all values within tolerance. self-test passed!
}
