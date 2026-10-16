/*
 * mpu6050.h
 *
 *  Created on: 14 Oct 2026
 *      Author: Billy Jones
 */

#ifndef CUSTOM_MPU6050_I2C_INC_MPU6050_H_
#define CUSTOM_MPU6050_I2C_INC_MPU6050_H_

#include <stdbool.h>

#include "i2c_bus.h"
#include "i2c_types.h"

typedef struct {
	int16_t accel_x_raw;
	int16_t accel_y_raw;
	int16_t accel_z_raw;
	int16_t temp_raw;
	int16_t gyro_x_raw;
	int16_t gyro_y_raw;
	int16_t gyro_z_raw;
} mpu6050_readings_raw;

typedef struct {
	float accel_x;
	float accel_y;
	float accel_z;
	float temp;
	float gyro_x;
	float gyro_y;
	float gyro_z;
} mpu6050_readings;

i2c_status_t mpu6050_init(uint16_t dev);

i2c_status_t mpu6050_get_readings(uint16_t dev, mpu6050_readings_raw* readings);

#endif /* CUSTOM_MPU6050_I2C_INC_MPU6050_H_ */
