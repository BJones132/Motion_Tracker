/*
 * mt_imu.h
 *
 *  Created on: 16 Oct 2026
 *      Author: Billy Jones
 */

#ifndef INC_MT_IMU_H_
#define INC_MT_IMU_H_

#include "mt_config.h"
#include "mt_types.h"
#include "mpu6050.h"

mt_status_t mt_imu_init(uint16_t dev);

mt_status_t mt_imu_read(mt_imu_data* data);

#endif /* INC_MT_IMU_H_ */
