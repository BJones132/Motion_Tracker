/*
 * mt_imu.c
 *
 *  Created on: 16 Oct 2026
 *      Author: Billy Jones
 */

#include "mt_imu.h"

uint16_t mt_imu_dev;

mt_status_t mt_imu_init(uint16_t dev){
	if(dev == 0x00U)
		return MT_PARAM_ERR;

	mt_imu_dev = dev;

	return mpu6050_init(dev, MT_MPU6050_SELF_TEST_ENABLE);
}

mt_status_t mt_imu_read(mt_imu_data* data){
	mpu6050_readings readings;
	mpu6050_status_t status = mpu6050_get_processed_readings(mt_imu_dev, &readings);
	if(status != MPU_OK)
		return MT_ERR;

	data->accel_x = readings.accel_x;
	data->accel_y = readings.accel_y;
	data->accel_z = readings.accel_z;
	data->gyro_x = readings.gyro_x;
	data->gyro_y = readings.gyro_y;
	data->gyro_z = readings.gyro_z;
	data->temp = readings.temp;

	return MT_OK;
}
