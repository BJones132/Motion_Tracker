/*
 * i2c_bus.h
 *
 *  Created on: 1 Oct 2026
 *      Author: Billy Jones
 */

#ifndef I2C_BUS_INC_I2C_BUS_H_
#define I2C_BUS_INC_I2C_BUS_H_

#include <stdbool.h>
#include "stm32f7xx_hal.h"

#include "i2c_types.h"
#include "i2c_config.h"

#define I2C_ADDRESS_8BIT(add)	((add) << 1)

i2c_status_t	i2c_bus_read(uint16_t dev, uint8_t reg, uint8_t* buf, uint16_t len);
i2c_status_t	i2c_bus_write(uint16_t dev, uint8_t reg, const uint8_t* buf, uint16_t len);
bool			i2c_bus_present(uint16_t dev);
bool			i2c_bus_recover(void); //TODO
bool			i2c_bus_isIdle(void); // TODO
int				i2c_bus_scanDevices(uint16_t first, uint16_t last, uint8_t* found, size_t cap);

#endif /* I2C_BUS_INC_I2C_BUS_H_ */
