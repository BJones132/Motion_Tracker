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
	mt_status_t status;
	if(first_update){
		status = sh1106_clear(mt_ui_dev);
		if(status != MT_OK)
			return status;

		const char headerString[] = "MOTION TRACKER";
		const char pitchChangeString[] = "Pitch:";
		const char rollChangeString[] = "Roll:";
		const char yawChangeString[] = "Yaw:";
		const char accelString[] = "Accel:";
		const char tempString[] = "Temp:";

		status = sh1106_drawString(mt_ui_dev, headerString, sizeof(headerString), 22, 7);
		if(status != MT_OK)
			return status;
		status = sh1106_drawString_aligned(mt_ui_dev, pitchChangeString, sizeof(pitchChangeString), 0, 5, SH1106_TEXT_ALIGN_LEFT);
		if(status != MT_OK)
			return status;
		status = sh1106_drawString_aligned(mt_ui_dev, rollChangeString, sizeof(rollChangeString), 0, 4, SH1106_TEXT_ALIGN_LEFT);
		if(status != MT_OK)
			return status;
		status = sh1106_drawString_aligned(mt_ui_dev, yawChangeString, sizeof(yawChangeString), 0, 3, SH1106_TEXT_ALIGN_LEFT);
		if(status != MT_OK)
			return status;
		status = sh1106_drawString_aligned(mt_ui_dev, accelString, sizeof(accelString), 0, 2, SH1106_TEXT_ALIGN_LEFT);
		if(status != MT_OK)
			return status;
		status = sh1106_drawString_aligned(mt_ui_dev, tempString, sizeof(tempString), 0, 1, SH1106_TEXT_ALIGN_LEFT);
		if(status != MT_OK)
			return status;
	}

	char pitchBuffer[5] = {0};
	snprintf(pitchBuffer, sizeof(pitchBuffer), "%.2f", data->gyro_x);
	char pitchString[7] = {
			pitchBuffer[0],
			pitchBuffer[1],
			pitchBuffer[2],
			pitchBuffer[3],
			0xB1U,
			'/',
			's'
	};
	status = sh1106_drawString_aligned(mt_ui_dev, (const char*)pitchString, sizeof(pitchString), 0, 5, SH1106_TEXT_ALIGN_RIGHT);
	if(status != MT_OK)
		return status;

	char rollBuffer[5] = {0};
	snprintf(rollBuffer, sizeof(rollBuffer), "%.2f", data->gyro_y);
	char rollString[7] = {
			rollBuffer[0],
			rollBuffer[1],
			rollBuffer[2],
			rollBuffer[3],
			0xB1U,
			'/',
			's'
	};
	status = sh1106_drawString_aligned(mt_ui_dev, (const char*)rollString, sizeof(rollString), 0, 4, SH1106_TEXT_ALIGN_RIGHT);
	if(status != MT_OK)
		return status;

	char yawBuffer[5] = {0};
	snprintf(yawBuffer, sizeof(yawBuffer), "%.2f", data->gyro_z);
	char yawString[7] = {
			yawBuffer[0],
			yawBuffer[1],
			yawBuffer[2],
			yawBuffer[3],
			0xB1U,
			'/',
			's'
	};
	status = sh1106_drawString_aligned(mt_ui_dev, (const char*)yawString, sizeof(yawString), 0, 3, SH1106_TEXT_ALIGN_RIGHT);
	if(status != MT_OK)
		return status;

	char accel_x_buffer[5] = {0};
	char accel_y_buffer[5] = {0};
	char accel_z_buffer[5] = {0};
	snprintf(accel_x_buffer, sizeof(accel_x_buffer), "%.2f", data->accel_x);
	snprintf(accel_y_buffer, sizeof(accel_y_buffer), "%.2f", data->accel_y);
	snprintf(accel_z_buffer, sizeof(accel_z_buffer), "%.2f", data->accel_z);
	char accelString[15] = {
			accel_x_buffer[0],
			accel_x_buffer[1],
			accel_x_buffer[2],
			accel_x_buffer[3],
			',',
			accel_y_buffer[0],
			accel_y_buffer[1],
			accel_y_buffer[2],
			accel_y_buffer[3],
			',',
			accel_z_buffer[0],
			accel_z_buffer[1],
			accel_z_buffer[2],
			accel_z_buffer[3],
			'g'
	};
	status = sh1106_drawString_aligned(mt_ui_dev, (const char*)accelString, sizeof(accelString), 0, 2, SH1106_TEXT_ALIGN_RIGHT);
	if(status != MT_OK)
		return status;

	char tempBuffer[6] = {0};
	snprintf(tempBuffer, sizeof(tempBuffer), "%.2f", data->temp);
	char tempString[7] = {
			tempBuffer[0],
			tempBuffer[1],
			tempBuffer[2],
			tempBuffer[3],
			tempBuffer[4],
			0xB1U,
			'C'
	};
	status = sh1106_drawString_aligned(mt_ui_dev, (const char*)tempString, sizeof(tempString), 0, 1, SH1106_TEXT_ALIGN_RIGHT);

	return status;
}


