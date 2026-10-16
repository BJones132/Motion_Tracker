/*
 * mt_types.h
 *
 *  Created on: 13 Oct 2026
 *      Author: Billy Jones
 */

#ifndef INC_MT_TYPES_H_
#define INC_MT_TYPES_H_

#include "stdint.h"

typedef struct {
	float accel_x;
	float accel_y;
	float accel_z;
	float temp;
	float gyro_x;
	float gyro_y;
	float gyro_z;
} mt_imu_data;

typedef enum{
	MT_OK = 0, MT_ERR, MT_PARAM_ERR, MT_EXITING = 8
} mt_status_t;

#endif /* INC_MT_TYPES_H_ */
