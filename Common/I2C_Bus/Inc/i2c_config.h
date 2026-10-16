/*
 * i2c_config.h
 *
 *  Created on: 8 Oct 2026
 *      Author: Billy Jones
 */

#ifndef I2C_BUS_INC_I2C_CONFIG_H_
#define I2C_BUS_INC_I2C_CONFIG_H_

#define I2C_BUS_HANDLE			(hi2c1)

#define I2C_BUS_TIMEOUT_MS		20U
#define I2C_BUS_TRIES			2U
#define I2C_BUS_RECOVER_PULSES	9U

#define I2C_BUS_SCL_PIN			GPIO_PIN_6
#define I2C_BUS_SDA_PIN			GPIO_PIN_9
#define I2C_BUS_GPIO_PORT		GPIOB

#endif /* I2C_BUS_INC_I2C_CONFIG_H_ */
