/*
 * mpu6050.c
 *
 *  Created on: 14 Oct 2026
 *      Author: Billy Jones
 */

#include "mpu6050.h"
#include "mpu6050_registers.h"

static void mpu6050_process_readings(mpu6050_readings_raw* raw_readings, mpu6050_readings* processed_readings){
	processed_readings->accel_x = (float)raw_readings->accel_x_raw / 4096;
	processed_readings->accel_y = (float)raw_readings->accel_y_raw / 4096;
	processed_readings->accel_z = (float)raw_readings->accel_z_raw / 4096;

	processed_readings->temp	= (float)raw_readings->temp_raw / 340 + 36.53;

	processed_readings->gyro_x	= (float)raw_readings->gyro_x_raw / 131;
	processed_readings->gyro_y	= (float)raw_readings->gyro_y_raw / 131;
	processed_readings->gyro_z	= (float)raw_readings->gyro_z_raw / 131;
}

bool mpu6050_selfTest(){

}

i2c_status_t mpu6050_init(uint16_t dev){
	if(dev == 0x00U)
		return I2C_PARAM_ERR;

	uint8_t pwr_mgmt_1_data[] = {0x01U};

	i2c_status_t status = i2c_bus_write(dev, MPU6050_REG_PWR_MGMT_1, pwr_mgmt_1_data, sizeof(pwr_mgmt_1_data));
	if(status != I2C_OK)
		return status;

	HAL_Delay(300); //Wait for MPU6050 to stabilise.

	uint8_t mpu6050_config_data[] = {
			0x00U,
			0x02U,
			0x00U,
			0x10U
	};

	status = i2c_bus_write(dev, MPU6050_REG_SMPRT_DIV, mpu6050_config_data, sizeof(mpu6050_config_data));
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

	uint8_t out[14] = {0};
	i2c_status_t status = i2c_bus_read(dev, MPU6050_REG_ACCEL_XOUT_H, out, sizeof(out));
	if(status != I2C_OK)
		return status;

	readings->accel_x_raw	= ((out[0]	<< 8) | out[1]);
	readings->accel_y_raw	= ((out[2]	<< 8) | out[3]);
	readings->accel_z_raw	= ((out[4]	<< 8) | out[5]);
	readings->temp_raw		= ((out[6]	<< 8) | out[7]);
	readings->gyro_x_raw	= ((out[8]	<< 8) | out[9]);
	readings->gyro_y_raw	= ((out[10]	<< 8) | out[11]);
	readings->gyro_z_raw	= ((out[12] << 8) | out[13]);

	return I2C_OK;
}




