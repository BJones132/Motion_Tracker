/*
 * mpu6050.c
 *
 *  Created on: 14 Oct 2026
 *      Author: Billy Jones
 */

#include "mpu6050.h"
#include "mpu6050_registers.h"
#include "mpu6050_types.h"
#include "mpu6050_config.h"

static void mpu6050_process_readings(mpu6050_readings_raw* raw_readings, mpu6050_readings* processed_readings){
	float accelConversion = 16384;
	if(MPU6050_ACCEL_CONFIG_FLAGS & MPU6050_ACCEL_CONFIG_FS_16)
		accelConversion = 2048;
	else if(MPU6050_ACCEL_CONFIG_FLAGS & MPU6050_ACCEL_CONFIG_FS_8)
		accelConversion = 4096;
	else if(MPU6050_ACCEL_CONFIG_FLAGS & MPU6050_ACCEL_CONFIG_FS_4)
		accelConversion = 8192;

	processed_readings->accel_x = (float)raw_readings->accel_x_raw / accelConversion;
	processed_readings->accel_y = (float)raw_readings->accel_y_raw / accelConversion;
	processed_readings->accel_z = (float)raw_readings->accel_z_raw / accelConversion;

	processed_readings->temp	= (float)raw_readings->temp_raw / 340 + 36.53; //Conversion to deg C from MPU6050 datasheet.

	float gyroConversion = 131;
	if(MPU6050_GYRO_CONFIG_FLAGS & MPU6050_GYRO_CONFIG_FS_2000)
		gyroConversion = 16.4;
	else if(MPU6050_GYRO_CONFIG_FLAGS & MPU6050_GYRO_CONFIG_FS_1000)
		gyroConversion = 32.8;
	else if(MPU6050_GYRO_CONFIG_FLAGS & MPU6050_GYRO_CONFIG_FS_500)
		gyroConversion = 65.5;

	processed_readings->gyro_x	= (float)raw_readings->gyro_x_raw / gyroConversion;
	processed_readings->gyro_y	= (float)raw_readings->gyro_y_raw / gyroConversion;
	processed_readings->gyro_z	= (float)raw_readings->gyro_z_raw / gyroConversion;
	//More processing if external sensor data is required
}

bool mpu6050_selfTest(){

}

i2c_status_t mpu6050_init(uint16_t dev){
	if(dev == 0x00U)
		return I2C_PARAM_ERR;

	const uint8_t pwr_mgmt_1_data[] = { MPU6050_PWR_MGMT_1_FLAGS };

	i2c_status_t status = i2c_bus_write(dev, MPU6050_REG_PWR_MGMT_1, pwr_mgmt_1_data, sizeof(pwr_mgmt_1_data));
	if(status != I2C_OK)
		return status;

	HAL_Delay(300); //Wait for MPU6050 to stabilise.

	const uint8_t mpu6050_config_data[] = {
			MPU6050_SMPRT_DIV_VAL,
			MPU6050_CONFIG_FLAGS,
			MPU6050_GYRO_CONFIG_FLAGS,
			MPU6050_ACCEL_CONFIG_FLAGS
	};

	status = i2c_bus_write(dev, MPU6050_REG_CONFIG_START, mpu6050_config_data, sizeof(mpu6050_config_data));
	if(status != I2C_OK)
		return status;

	mpu6050_readings_raw readings;
	status = mpu6050_get_readings(dev, &readings);
	if(status != I2C_OK)
		return status;

	mpu6050_readings processed_readings;
	mpu6050_process_readings(&readings, &processed_readings);

	return status;
}

i2c_status_t mpu6050_get_readings(uint16_t dev, mpu6050_readings_raw* readings){
	if(dev == 0x00U)
		return I2C_PARAM_ERR;

	uint8_t out[(MPU6050_REG_DATA_OUT_END - MPU6050_REG_DATA_OUT_START) + 1] = {0};
	i2c_status_t status = i2c_bus_read(dev, MPU6050_REG_DATA_OUT_START, out, sizeof(out));
	if(status != I2C_OK)
		return status;

	readings->accel_x_raw	= ((out[0]	<< 8) | out[1]);
	readings->accel_y_raw	= ((out[2]	<< 8) | out[3]);
	readings->accel_z_raw	= ((out[4]	<< 8) | out[5]);
	readings->temp_raw		= ((out[6]	<< 8) | out[7]);
	readings->gyro_x_raw	= ((out[8]	<< 8) | out[9]);
	readings->gyro_y_raw	= ((out[10]	<< 8) | out[11]);
	readings->gyro_z_raw	= ((out[12] << 8) | out[13]);
	//More readings would be taken if external sensor data is required

	return I2C_OK;
}




