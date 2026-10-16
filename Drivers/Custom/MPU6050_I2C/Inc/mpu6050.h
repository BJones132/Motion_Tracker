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

#include "mpu6050_types.h"

mpu6050_status_t mpu6050_init(uint16_t dev, bool self_test_enable);

mpu6050_status_t mpu6050_get_readings(uint16_t dev, mpu6050_readings_raw* readings);
mpu6050_status_t mpu6050_get_processed_readings(uint16_t dev, mpu6050_readings* processed_readings);

#endif /* CUSTOM_MPU6050_I2C_INC_MPU6050_H_ */
