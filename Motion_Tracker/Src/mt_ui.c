/*
 * mt_ui.c
 *
 *  Created on: 13 Oct 2026
 *      Author: Billy Jones
 */

#include "mt_ui.h"
#include <stdio.h>

uint16_t mt_ui_dev;

/**
  * @brief	(LOCAL) Display the setting up screen to the user.
  * @retval void
  */
static i2c_status_t show_setup_screen(){
	if(mt_ui_dev == 0)
		return I2C_PARAM_ERR;

	const char setupString[] = "Setting Up...";
	i2c_status_t status = sh1106_drawString(mt_ui_dev, setupString, sizeof(setupString), 25, 4);
	if(status != I2C_OK)
		return status;

	if(MT_MPU6050_SELF_TEST_ENABLE){
		const char dontMoveString[] = "Don't Move Device";
		status = sh1106_drawString(mt_ui_dev, dontMoveString, sizeof(dontMoveString), 18,2);
		if(status != I2C_OK)
			return status;
	}else{
		const char selfTestDisabledString[] = "Self Test Disabled";
		status = sh1106_drawString(mt_ui_dev, selfTestDisabledString, sizeof(selfTestDisabledString), 10,2);
		if(status != I2C_OK)
			return status;
		const char selfTestDisabledWarningString[] = "Inaccurate Data";
		status = sh1106_drawString(mt_ui_dev, selfTestDisabledWarningString, sizeof(selfTestDisabledWarningString), 19,1);
		if(status != I2C_OK)
			return status;
	}

	return I2C_OK;
}

/**
  * @brief	Draw an aligned/justified string of characters to display.
  * @param  dev			Address of device to draw to.
  * @retval mt_status_t	Status
  */
mt_status_t mt_ui_init(uint16_t dev){
	if(dev == 0)
		return MT_PARAM_ERR;
	mt_ui_dev = dev;

	i2c_status_t status = sh1106_init(dev);

	//TODO: Ensure all errors are covered
	switch(status){
		case(I2C_ERR):
			return MT_ERR;
		case(I2C_PARAM_ERR):
			return MT_PARAM_ERR;
	}

	status = show_setup_screen();

	//TODO: Ensure all errors are covered
	switch(status){
		case(I2C_ERR):
			return MT_ERR;
		case(I2C_PARAM_ERR):
			return MT_PARAM_ERR;
	}

	return MT_OK;
}

mt_status_t mt_ui_update(mt_imu_data* data, bool first_update){
	if(first_update){
		sh1106_clear(mt_ui_dev);
		const char headerString[] = "MOTION TRACKER";
		const char pitchChangeString[] = "Pitch:";
		const char rollChangeString[] = "Roll:";
		const char yawChangeString[] = "Yaw:";
		const char accelString[] = "Accel:";
		const char tempString[] = "Temp:";

		sh1106_drawString(mt_ui_dev, headerString, sizeof(headerString), 22, 7);
		sh1106_drawString_aligned(mt_ui_dev, pitchChangeString, sizeof(pitchChangeString), 0, 5, SH1106_TEXT_ALIGN_LEFT);
		sh1106_drawString_aligned(mt_ui_dev, rollChangeString, sizeof(rollChangeString), 0, 4, SH1106_TEXT_ALIGN_LEFT);
		sh1106_drawString_aligned(mt_ui_dev, yawChangeString, sizeof(yawChangeString), 0, 3, SH1106_TEXT_ALIGN_LEFT);
		sh1106_drawString_aligned(mt_ui_dev, accelString, sizeof(accelString), 0, 2, SH1106_TEXT_ALIGN_LEFT);
		sh1106_drawString_aligned(mt_ui_dev, tempString, sizeof(tempString), 0, 1, SH1106_TEXT_ALIGN_LEFT);
	}

	char pitchBuffer[4] = {0};
	int16_t intPitch = (int16_t)data->gyro_x;
	snprintf(pitchBuffer, sizeof(pitchBuffer), "%d", intPitch);
	char pitchString[7] = {
			pitchBuffer[0],
			pitchBuffer[1],
			pitchBuffer[2],
			pitchBuffer[3],
			0xB1U,
			'/',
			's'
	};
	sh1106_drawString_aligned(mt_ui_dev, pitchString, sizeof(pitchString), 0, 5, SH1106_TEXT_ALIGN_RIGHT);

	char rollBuffer[4] = {0};
	int16_t intRoll = (int16_t)data->gyro_y;
	snprintf(rollBuffer, sizeof(rollBuffer), "%d", intRoll);
	char rollString[7] = {
			rollBuffer[0],
			rollBuffer[1],
			rollBuffer[2],
			rollBuffer[3],
			0xB1U,
			'/',
			's'
	};
	sh1106_drawString_aligned(mt_ui_dev, rollString, sizeof(rollString), 0, 4, SH1106_TEXT_ALIGN_RIGHT);

	char yawBuffer[4] = {0};
	int16_t intYaw = (int16_t)data->gyro_z;
	snprintf(yawBuffer, sizeof(yawBuffer), "%d", intYaw);
	char yawString[7] = {
			yawBuffer[0],
			yawBuffer[1],
			yawBuffer[2],
			yawBuffer[3],
			0xB1U,
			'/',
			's'
	};
	sh1106_drawString_aligned(mt_ui_dev, yawString, sizeof(yawString), 0, 3, SH1106_TEXT_ALIGN_RIGHT);

	char accel_x_buffer[3] = {' ',' ',' '};
	char accel_y_buffer[3] = {' ',' ',' '};
	char accel_z_buffer[3] = {' ',' ',' '};
	int16_t intAccelX = (int16_t)data->accel_x;
	int16_t intAccelY = (int16_t)data->accel_y;
	int16_t intAccelZ = (int16_t)data->accel_z;
	snprintf(accel_x_buffer, sizeof(accel_x_buffer), "%d", intAccelX);
	snprintf(accel_y_buffer, sizeof(accel_y_buffer), "%d", intAccelY);
	snprintf(accel_z_buffer, sizeof(accel_z_buffer), "%d", intAccelZ);
	char accelString[12] = {
			accel_x_buffer[0],
			accel_x_buffer[1],
			accel_x_buffer[2],
			',',
			accel_y_buffer[0],
			accel_y_buffer[1],
			accel_y_buffer[2],
			',',
			accel_z_buffer[0],
			accel_z_buffer[1],
			accel_z_buffer[2],
			'g'
	};
	sh1106_drawString_aligned(mt_ui_dev, accelString, sizeof(accelString), 0, 2, SH1106_TEXT_ALIGN_RIGHT);

	char tempBuffer[4] = {0};
	int16_t intTemp = (int16_t)data->temp;
	snprintf(tempBuffer, sizeof(tempBuffer), "%d", intTemp);
	char tempString[6] = {
			tempBuffer[0],
			tempBuffer[1],
			tempBuffer[2],
			tempBuffer[3],
			0xB1U,
			'C'
	};
	sh1106_drawString_aligned(mt_ui_dev, tempString, sizeof(tempString), 0, 1, SH1106_TEXT_ALIGN_RIGHT);
}


