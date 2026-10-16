/*
 * i2c_types.h
 *
 *  Created on: 1 Oct 2026
 *      Author: Billy Jones
 */

#ifndef I2C_BUS_INC_I2C_TYPES_H_
#define I2C_BUS_INC_I2C_TYPES_H_

typedef enum{
	I2C_OK = 0, I2C_ERR, I2C_BUSY, I2C_TIMEOUT, I2C_PARAM_ERR
} i2c_status_t;

#endif /* I2C_BUS_INC_I2C_TYPES_H_ */
