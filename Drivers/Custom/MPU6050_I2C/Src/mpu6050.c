/*
 * mpu6050.c
 *
 *  Created on: 14 Oct 2026
 *      Author: Billy Jones
 */

#include <math.h>

#include "mpu6050.h"
#include "mpu6050_registers.h"
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

typedef struct{
	uint8_t self_test_x;
	uint8_t self_test_y;
	uint8_t self_test_z;
	uint8_t self_test_a;
} mpu6050_self_test_readings;

typedef struct{
	float ftxg;
	float ftyg;
	float ftzg;

	float ftxa;
	float ftya;
	float ftza;
} mpu6050_self_test_factory_trims;

static mpu6050_status_t mpu6050_get_self_test_readings(uint16_t dev, mpu6050_self_test_readings* readings){
	if(dev == 0x00U)
		return I2C_PARAM_ERR;

	uint8_t out[(MPU6050_REG_SELF_TEST_END - MPU6050_REG_SELF_TEST_START) + 1] = { 0 };
	i2c_status_t status = i2c_bus_read(dev, MPU6050_REG_SELF_TEST_START, out, sizeof(out));
	if(status != I2C_OK)
		return status;

	readings->self_test_x = out[0];
	readings->self_test_y = out[1];
	readings->self_test_z = out[2];
	readings->self_test_a = out[3];

	return I2C_OK;
}

static void mpu6050_get_self_test_factory_trims(mpu6050_self_test_readings* readings, mpu6050_self_test_factory_trims* trims){
	uint8_t stxg = readings->self_test_x & 0x0FU;
	uint8_t styg = readings->self_test_y & 0x0FU;
	uint8_t stzg = readings->self_test_z & 0x0FU;
	uint8_t stxa = (((readings->self_test_x & 0xE0U) >> 3) + ((readings->self_test_a & 0x30U) >> 4));
	uint8_t stya = (((readings->self_test_y & 0xE0U) >> 3) + ((readings->self_test_a & 0x0CU) >> 2));
	uint8_t stza = (((readings->self_test_z & 0xE0U) >> 3) + (readings->self_test_a & 0x03U));

	trims->ftxg = 0;
	if(stxg != 0)
		trims->ftxg = (25 * 131 * powf(1.046, (float)stxg-1));

	trims->ftyg = 0;
	if(styg != 0)
		trims->ftyg = (-25 * 131 * powf(1.046, (float)styg-1));

	trims->ftzg = 0;
	if(stzg != 0)
		trims->ftzg = (25 * 131 * powf(1.046, (float)stzg-1));

	trims->ftxa = 0;
	if(stxa != 0)
		trims->ftxa = (4096 * 0.34 * powf(0.92 / 0.34, ((float)stxa - 1) / (powf(2, 5) - 2)));

	trims->ftya = 0;
	if(stya != 0)
		trims->ftya = (4096 * 0.34 * powf(0.92 / 0.34, ((float)stya - 1) / (powf(2, 5) - 2)));

	trims->ftza = 0;
	if(stza != 0)
		trims->ftza = (4096 * 0.34 * powf(0.92 / 0.34, ((float)stza - 1) / (powf(2, 5) - 2)));
}

static mpu6050_status_t mpu6050_get_avg_readings(uint16_t dev, mpu6050_readings_raw* readings){
	int32_t avg_accel_x = 0;
	int32_t avg_accel_y = 0;
	int32_t avg_accel_z = 0;
	int32_t avg_gyro_x = 0;
	int32_t avg_gyro_y = 0;
	int32_t avg_gyro_z = 0;

	mpu6050_readings_raw sample_readings;
	sample_readings.temp_raw = 0;
	i2c_status_t status;
	for(int i = 0; i < MPU6050_SELF_TEST_SAMPLES; i++){
		status = mpu6050_get_readings(dev, &sample_readings);
		if(status != I2C_OK)
			return status;

		avg_accel_x += sample_readings.accel_x_raw;
		avg_accel_y += sample_readings.accel_y_raw;
		avg_accel_z += sample_readings.accel_z_raw;
		avg_gyro_x += sample_readings.gyro_x_raw;
		avg_gyro_y += sample_readings.gyro_y_raw;
		avg_gyro_z += sample_readings.gyro_z_raw;
		HAL_Delay(10);
	}
	avg_accel_x /= MPU6050_SELF_TEST_SAMPLES;
	avg_accel_y /= MPU6050_SELF_TEST_SAMPLES;
	avg_accel_z /= MPU6050_SELF_TEST_SAMPLES;
	avg_gyro_x /= MPU6050_SELF_TEST_SAMPLES;
	avg_gyro_y /= MPU6050_SELF_TEST_SAMPLES;
	avg_gyro_z /= MPU6050_SELF_TEST_SAMPLES;

	readings->accel_x_raw	=	(int16_t)avg_accel_x;
	readings->accel_y_raw	=	(int16_t)avg_accel_y;
	readings->accel_z_raw	=	(int16_t)avg_accel_z;
	readings->gyro_x_raw	=	(int16_t)avg_gyro_x;
	readings->gyro_y_raw	=	(int16_t)avg_gyro_y;
	readings->gyro_z_raw	=	(int16_t)avg_gyro_z;

	return I2C_OK;
}

static bool mpu6050_self_test_pass(mpu6050_readings_raw* baseline_readings,
				mpu6050_readings_raw* self_test_readings,
				mpu6050_self_test_factory_trims* factory_trims){
	mpu6050_readings_raw self_test_response;
	self_test_response.temp_raw = 0;
	self_test_response.accel_x_raw = self_test_readings->accel_x_raw - baseline_readings->accel_x_raw;
	self_test_response.accel_y_raw = self_test_readings->accel_y_raw - baseline_readings->accel_y_raw;
	self_test_response.accel_z_raw = self_test_readings->accel_z_raw - baseline_readings->accel_z_raw;
	self_test_response.gyro_x_raw = self_test_readings->gyro_x_raw - baseline_readings->gyro_x_raw;
	self_test_response.gyro_y_raw = self_test_readings->gyro_y_raw - baseline_readings->gyro_y_raw;
	self_test_response.gyro_z_raw = self_test_readings->gyro_z_raw - baseline_readings->gyro_z_raw;

	float accel_x_change = (((float)self_test_response.accel_x_raw - factory_trims->ftxa) / factory_trims->ftxa);
	if(accel_x_change > 0.14 || accel_x_change < -0.14)
		return false;

	float accel_y_change = (((float)self_test_response.accel_y_raw - factory_trims->ftya) / factory_trims->ftya);
	if(accel_y_change > 0.14 || accel_y_change < -0.14)
		return false;

	float accel_z_change = (((float)self_test_response.accel_z_raw - factory_trims->ftza) / factory_trims->ftza);
	if(accel_z_change > 0.14 || accel_z_change < -0.14)
		return false;

	float gyro_x_change = (((float)self_test_response.gyro_x_raw - factory_trims->ftxg) / factory_trims->ftxg);
	if(gyro_x_change > 0.14 || gyro_x_change < -0.14)
		return false;

	float gyro_y_change = (((float)self_test_response.gyro_y_raw - factory_trims->ftyg) / factory_trims->ftyg);
	if(gyro_y_change > 0.14 || gyro_y_change < -0.14)
		return false;

	float gyro_z_change = (((float)self_test_response.gyro_z_raw - factory_trims->ftzg) / factory_trims->ftzg);
	if(gyro_z_change > 0.14 || gyro_z_change < -0.14)
		return false;

	return true;
}

static bool mpu6050_self_test(uint16_t dev){
	mpu6050_self_test_readings st_readings;
	if(mpu6050_get_self_test_readings(dev, &st_readings) != I2C_OK)
		return false;
	mpu6050_self_test_factory_trims factory_trims;
	mpu6050_get_self_test_factory_trims(&st_readings, &factory_trims);

	mpu6050_readings_raw avg_baseline_readings;
	if(mpu6050_get_avg_readings(dev, &avg_baseline_readings) != I2C_OK)
		return false;

	const uint8_t config_data_st[] = {
			MPU6050_GYRO_CONFIG_FLAGS_ST,
			MPU6050_ACCEL_CONFIG_FLAGS_ST
	};
	if(i2c_bus_write(dev, MPU6050_REG_GYRO_CONFIG, config_data_st, sizeof(config_data_st)) != I2C_OK)
		return false;

	HAL_Delay(300); //Wait for MPU6050 to stabilise.

	mpu6050_readings_raw avg_self_test_readings;
	if(mpu6050_get_avg_readings(dev, &avg_self_test_readings) != I2C_OK)
		return false;

	const uint8_t config_data_default[] = {
			MPU6050_GYRO_CONFIG_FLAGS,
			MPU6050_ACCEL_CONFIG_FLAGS
	};
	if(i2c_bus_write(dev, MPU6050_REG_GYRO_CONFIG, config_data_default, sizeof(config_data_default)) != I2C_OK)
		return false;

	return mpu6050_self_test_pass(&avg_baseline_readings, &avg_self_test_readings, &factory_trims);
}

mpu6050_status_t mpu6050_init(uint16_t dev, bool self_test_enable){
	if(dev == 0x00U)
		return MPU_PARAM_ERR;

	const uint8_t pwr_mgmt_1_data[] = { MPU6050_PWR_MGMT_1_FLAGS };

	i2c_status_t status = i2c_bus_write(dev, MPU6050_REG_PWR_MGMT_1, pwr_mgmt_1_data, sizeof(pwr_mgmt_1_data));
	if(status != I2C_OK)
		return status;

	const uint8_t config_data[] = {
			MPU6050_SMPRT_DIV_VAL,
			MPU6050_CONFIG_FLAGS,
			MPU6050_GYRO_CONFIG_FLAGS,
			MPU6050_ACCEL_CONFIG_FLAGS
	};

	status = i2c_bus_write(dev, MPU6050_REG_CONFIG_START, config_data, sizeof(config_data));
	if(status != I2C_OK)
		return status;

	HAL_Delay(300);

	if(self_test_enable){
		if(mpu6050_self_test(dev) == false)
			return MPU_SELF_TEST_FAIL;
	}

	mpu6050_readings_raw readings;
	status = mpu6050_get_readings(dev, &readings);
	if(status != I2C_OK)
		return status;

	mpu6050_readings processed_readings;
	mpu6050_process_readings(&readings, &processed_readings);

	return status;
}

mpu6050_status_t mpu6050_get_readings(uint16_t dev, mpu6050_readings_raw* readings){
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

	return MPU_OK;
}

mpu6050_status_t mpu6050_get_processed_readings(uint16_t dev, mpu6050_readings* processed_readings){
	mpu6050_readings_raw readings;
	mpu6050_status_t status = mpu6050_get_readings(dev, &readings);
	if(status != MPU_OK)
		return status;

	mpu6050_process_readings(&readings, processed_readings);

	return MPU_OK;
}



