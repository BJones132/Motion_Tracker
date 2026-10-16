/*
 * mt_main.c
 *
 *  Created on: 13 Oct 2026
 *      Author: Billy Jones
 */

#include "mt_main.h"
#include "mt_ui.h"
#include "mpu6050.h"

uint8_t mt_sh1106_addr = 0;
uint8_t mt_mpu6050_addr = 0;

/**
  * @brief	(LOCAL) Find the pre-configured devices on the i2c bus.
  * @retval void
  */
static void mt_findDevices(void){
	uint8_t found[2] = {0};
	  size_t count = i2c_bus_scanDevices(MT_SH1106_ADDR, MT_MPU6050_ADDR, found, sizeof(found));
	  for(int i = 0; i < count && i < sizeof(found); i++){
		  if(found[i] == MT_SH1106_ADDR){
			  mt_sh1106_addr = found[i];
			  continue;
		  }else if(found[i] == MT_MPU6050_ADDR){
			  mt_mpu6050_addr = found[i];
			  continue;
		  }
	  }
}

/**
  * @brief	Initialise the motion tracker.
  * @retval mt_status_t	Status
  */
mt_status_t mt_init(void){
	mt_findDevices();

	if(mt_sh1106_addr == 0)
		return MT_ERR;

	mt_status_t status = mt_ui_init(mt_sh1106_addr);
	if(status != MT_OK)
		return status;

	status = mpu6050_init(mt_mpu6050_addr);
	return status;
}

