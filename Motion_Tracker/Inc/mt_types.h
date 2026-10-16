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
	uint16_t accel_x;
	uint16_t accel_y;
	uint16_t accel_z;

	uint16_t temp;

	uint16_t gyro_x;
	uint16_t gyro_y;
	uint16_t gyro_z;
} mt_imu_data;

typedef enum{
	MT_OK = 0, MT_ERR, MT_PARAM_ERR, MT_EXITING = 8
} mt_status_t;

#endif /* INC_MT_TYPES_H_ */
