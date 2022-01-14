#include <msp430.h>
#include <stdint.h>
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
    uint8_t data[] = {DEVICE_RESET};
    I2C_WriteBuffer(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, data, 1);
    do{
        I2C_ReadReg(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1,1);
        while(i2c.MasterMode != IDLE_MODE);
    }while(i2c.RXBuffer[0] != 0x40); // Wait for the the device reset to complete

    // Wake up the MPU6050
    data[0] = 0x00; // clears the sleep bit
    I2C_WriteBuffer(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, data, 1);

    // Set sample rate
    data[0] = 0x00;
    I2C_WriteBuffer(MPU6050_ADDRESS, MPU6050_SMPLRT_DIV, data, 1);

    // Configure low pass filter
    data[0] = EXT_SYNC_SET_INPUT_DISABLE + DLPF_CFG_BAND_WIDTH_10HZ;
    I2C_WriteBuffer(MPU6050_ADDRESS, MPU6050_CONFIG, data, 1);

    // Configure accelerometer range
    data[0] = AFS_SEL_SCALE_2G;
    I2C_WriteBuffer(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, data, 1);

    // Congfigure gyroscope range
    data[0] = PS_SEL_SCALE_250;
    I2C_WriteBuffer(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, data, 1);

    // Select clock source
    data[0] = CLKSEL_3;
    I2C_WriteBuffer(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, data, 1);

    // TODO Perform self-test? (see accel_config and gyro_config registers)
}

/*--------------------------------------------------------------------------------
Function    : MPU6050_CheckI2C
Purpose     : Check I2C communication
Parameters  : None
Return      : Value of WHO_AM_I registor (0x68)
-------------------------------------------------------------------------------*/
uint8_t MPU6050_CheckI2C(void)
{
        I2C_ReadReg(MPU6050_ADDRESS, MPU6050_WHO_AM_I, 1);
        while(i2c.MasterMode != IDLE_MODE);
        return i2c.RXBuffer[0];
}

/*--------------------------------------------------------------------------------
Function    : MPU6050_TestRegConfig
Purpose     : Check the config of key registers:
Parameters  : None
Return      : 0 if pass and 1 if fail
-------------------------------------------------------------------------------*/

uint8_t MPU6050_TestRegConfig(void)
{
        uint8_t status = 0;
        I2C_ReadReg(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1,1);
        while(i2c.MasterMode != IDLE_MODE);
        status =  (i2c.RXBuffer[0] == CLKSEL_3 ? 0: 1) || status;

        I2C_ReadReg(MPU6050_ADDRESS, MPU6050_SMPLRT_DIV,1);
        while(i2c.MasterMode != IDLE_MODE);
        status =  (i2c.RXBuffer[0] == 0x00 ? 0: 1) || status;

        I2C_ReadReg(MPU6050_ADDRESS, MPU6050_CONFIG,1);
        while(i2c.MasterMode != IDLE_MODE);
        status =  (i2c.RXBuffer[0] == EXT_SYNC_SET_INPUT_DISABLE + DLPF_CFG_BAND_WIDTH_10HZ ? 0: 1) || status;

        I2C_ReadReg(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG,1);
        while(i2c.MasterMode != IDLE_MODE);
        status =  (i2c.RXBuffer[0] == 0x00 ? 0: 1) || status;

        I2C_ReadReg(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG,1);
        while(i2c.MasterMode != IDLE_MODE);
        status =  (i2c.RXBuffer[0] == 0x00 ? 0: 1) || status;

        return status;
}
