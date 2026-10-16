/*
 * i2c_bus.c
 *
 *  Created on: 8 Oct 2026
 *      Author: Billy Jones
 */

#include "i2c_bus.h"

extern I2C_HandleTypeDef I2C_BUS_HANDLE;

/**
  * @brief	Check if device is present on i2c bus.
  * @param	dev	Address of device to check for.
  * @retval	bool
  */
bool i2c_bus_present(uint16_t dev){
	i2c_status_t status = (i2c_status_t)HAL_I2C_IsDeviceReady(&I2C_BUS_HANDLE, I2C_ADDRESS_8BIT(dev), I2C_BUS_TRIES, I2C_BUS_TIMEOUT_MS);

	switch(status){
		case I2C_OK:
			return true;
		case I2C_ERR:
			return false;
		case I2C_BUSY:
			//recover bus and attempt again, add a retry amount
		case I2C_TIMEOUT:
			//Log error
			return false;
		default:
			//Log undefined behaviour
			return false;
	}
}

/**
  * @brief	Read data from register on device.
  * @param  dev	Address of device to read from.
  * @param	reg	Register on device to read from.
  * @param	buf	Output character buffer.
  * @param	len	Amount of data to read.
  * @retval i2c_status_t	Status
  */
i2c_status_t i2c_bus_read(uint16_t dev, uint8_t reg, uint8_t* buf, uint16_t len){
	i2c_status_t status = (i2c_status_t)HAL_I2C_Mem_Read(&I2C_BUS_HANDLE, I2C_ADDRESS_8BIT(dev), reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)buf, len, I2C_BUS_TIMEOUT_MS);

	switch(status){
		case I2C_BUSY:
			//recover bus and attempt again, add a retry amount
		default:
			return status;
	}
}

/**
  * @brief	Write data to register on device.
  * @param  dev	Address of device to write to.
  * @param	reg	Register on device to write to.
  * @param	buf	Input character buffer.
  * @param	len	Amount of data to write, usually buf length.
  * @retval i2c_status_t	Status
  */
i2c_status_t i2c_bus_write(uint16_t dev, uint8_t reg, const uint8_t* buf, uint16_t len){
	i2c_status_t status = (i2c_status_t)HAL_I2C_Mem_Write(&I2C_BUS_HANDLE, I2C_ADDRESS_8BIT(dev), reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)buf, len, I2C_BUS_TIMEOUT_MS);

	switch(status){
		case I2C_BUSY:
			//recover bus and attempt again, add a retry amount
		default:
			return status;
	}
}

/**
  * @brief	Scan for devices in range.
  * @param  first	Starting address for range.
  * @param	last	Final address for range.
  * @param	found	Output array of found addresses.
  * @param	cap		Maxumim amount of found addresses.
  * @retval int		Number of devices found on bus (uncapped)
  */
int i2c_bus_scanDevices(uint16_t first, uint16_t last, uint8_t* found, size_t cap){
	size_t count = 0;
	for(uint16_t addr = first; addr <= last; addr++){
		if(i2c_bus_present(addr)){
			if(found && count < cap)
				found[count] = (uint8_t)addr;
			count++;
		}
	}
	return count;
}
