/*
 * sh1106.c
 *
 *  Created on: 10 Oct 2026
 *      Author: Billy Jones
 */

#include "sh1106.h"

/**
  * @brief	Initialise display to defaults.
  * @param  dev				Address of device to initialise.
  * @retval i2c_status_t	Status
  */
i2c_status_t sh1106_init(uint16_t dev){
	if(dev == 0x00U)
		return I2C_PARAM_ERR;

	const uint8_t init_buf[] = {
			SH1106_COM_ADC_REMAP_RIGHT,
			SH1106_COM_COMPADS_MODE_SET,
			SH1106_DAT_COMPADS_MODE_ALT,
			SH1106_COM_OUTPUT_SCAN_DIR_STD,
			SH1106_COM_INVERT_DISPLAY_OFF,
			SH1106_COM_MULTI_RATION_SET,
			SH1106_DAT_MULTI_RATION_DEFAULT,
			SH1106_COM_CLOCK_SET,
			SH1106_DAT_CLOCK_DEFAULT,
			SH1106_COM_VCOM_DSEL_LEVEL_SET,
			SH1106_DAT_VCOM_DSEL_LEVEL_DEFAULT,
			SH1106_COM_CONTRAST_SET,
			SH1106_DAT_CONTRAST_DEFAULT,
			SH1106_COM_PUMP_VOLT_DEFAULT,
			SH1106_COM_DC_PUMP_SET,
			SH1106_DAT_DC_PUMP_ON
	};

	i2c_status_t status = i2c_bus_write(dev, SH1106_COMMAND_CONT, init_buf, sizeof(init_buf));
	if(status != I2C_OK)
		return status;

	status = sh1106_clear(dev);
	if(status != I2C_OK)
		return status;

	const uint8_t displayOn[] = { SH1106_COM_DISPLAY_ON };
	status = i2c_bus_write(dev, SH1106_COMMAND_NON_CONT, displayOn, sizeof(displayOn));

	return status;
}

/**
  * @brief	Clear display.
  * @param  dev				Address of device to clear.
  * @retval i2c_status_t	Status
  */
i2c_status_t sh1106_clear(uint16_t dev){
	uint8_t setColsBuf[2];

	sh1106_setColAddressCommand(0, setColsBuf, false);

	i2c_status_t status = i2c_bus_write(dev, SH1106_COMMAND_CONT, setColsBuf, sizeof(setColsBuf));
	if(status != I2C_OK)
		return status;

	const uint8_t zeroFill[SH1106_DISPLAY_WIDTH_PX_FULL]={ 0 };

	const uint8_t clearEndBuf[] = {
				SH1106_COM_RWM_END
	};

	for(int i = 0; i < SH1106_DISPLAY_HEIGHT_PAGE; i++){
		const uint8_t clearStartBuf[] = {
				SH1106_COM_PAGE_ADDR_SET(i),
				SH1106_COM_RWM_START
		};

		status = i2c_bus_write(dev, SH1106_COMMAND_CONT, clearStartBuf, sizeof(clearStartBuf));
		if(status != I2C_OK)
			return status;

		status = i2c_bus_write(dev, SH1106_DATA_CONT, zeroFill, sizeof(zeroFill));
		if(status != I2C_OK)
			return status;

		status = i2c_bus_write(dev, SH1106_COMMAND_NON_CONT, clearEndBuf, sizeof(clearEndBuf));
		if(status != I2C_OK)
			return status;
	}

	sh1106_setColAddressCommand(0, setColsBuf, true);

	status = i2c_bus_write(dev, SH1106_COMMAND_CONT, setColsBuf, sizeof(setColsBuf));

	return status;
}

/**
  * @brief	Draw a single glyph to display.
  * @param  dev				Address of device to draw to.
  * @param	glyph			Glyph character to draw.
  * @param	x				X coordinate to draw at.
  * @param	page			Page to draw at.
  * @retval i2c_status_t	Status
  */
i2c_status_t sh1106_drawGlyph(uint16_t dev, const char glyph, uint8_t x, uint8_t page){
	if(x > (SH1106_DISPLAY_WIDTH_PX - SH1106_FONT_WIDTH) || page > (SH1106_DISPLAY_HEIGHT_PAGE - 1)){
		return I2C_PARAM_ERR;
	}

	uint8_t setColBuf[2];
	sh1106_setColAddressCommand(x, setColBuf, true);

	const uint8_t beginDrawBuf[] = {
			setColBuf[0],
			setColBuf[1],
			SH1106_COM_PAGE_ADDR_SET(page),
			SH1106_COM_RWM_START
	};

	i2c_status_t status = i2c_bus_write(dev, SH1106_COMMAND_CONT, beginDrawBuf, sizeof(beginDrawBuf));
	if(status != I2C_OK)
		return status;

	status = i2c_bus_write(dev, SH1106_DATA_CONT, sh1106_portfolio_6x8[(unsigned char)glyph], sizeof(sh1106_portfolio_6x8[(unsigned char)glyph]));
	if(status != I2C_OK)
		return status;

	sh1106_setColAddressCommand(0, setColBuf, true);

	const uint8_t endDrawBuf[] = {
			SH1106_COM_RWM_END,
			setColBuf[0],
			setColBuf[1],
			SH1106_COM_PAGE_ADDR_SET(0)
	};

	status = i2c_bus_write(dev, SH1106_COMMAND_CONT, endDrawBuf, sizeof(endDrawBuf));

	return status;
}

/**
  * @brief	Draw a string of characters to display.
  * @param  dev				Address of device to draw to.
  * @param	string			Character string to draw.
  * @param	len				Length of string.
  * @param	x				X coordinate to draw at.
  * @param	page			Page to draw at.
  * @retval i2c_status_t	Status
  */
i2c_status_t sh1106_drawString(uint16_t dev, const char* string, size_t len, uint8_t x, uint8_t page){
	if(x > (SH1106_DISPLAY_WIDTH_PX - (SH1106_FONT_WIDTH*len)) || page > (SH1106_DISPLAY_HEIGHT_PAGE - 1)){
		return I2C_PARAM_ERR;
	}

	uint8_t setColBuf[2];
	sh1106_setColAddressCommand(x, setColBuf, true);

	const uint8_t beginDrawBuf[] = {
			setColBuf[0],
			setColBuf[1],
			SH1106_COM_PAGE_ADDR_SET(page),
			SH1106_COM_RWM_START
	};

	unsigned char* drawBuf = calloc(len, sizeof(unsigned char[6]));
	for(int i = 0; i < len; i++){
		for(int j = 0; j < SH1106_FONT_WIDTH; j++){
			drawBuf[(i * SH1106_FONT_WIDTH) + j] = sh1106_portfolio_6x8[(unsigned char)string[i]][j];
		}
	}

	i2c_status_t status = i2c_bus_write(dev, SH1106_COMMAND_CONT, beginDrawBuf, sizeof(beginDrawBuf));
	if(status != I2C_OK)
		return status;

	status = i2c_bus_write(dev, SH1106_DATA_CONT, drawBuf, sizeof(unsigned char[6])*len);
	if(status != I2C_OK)
		return status;

	sh1106_setColAddressCommand(0, setColBuf, true);

	const uint8_t endDrawBuf[] = {
			SH1106_COM_RWM_END,
			setColBuf[0],
			setColBuf[1],
			SH1106_COM_PAGE_ADDR_SET(0)
	};

	status = i2c_bus_write(dev, SH1106_COMMAND_CONT, endDrawBuf, sizeof(endDrawBuf));

	return status;
}

/**
  * @brief	Draw an aligned/justified string of characters to display.
  * @param  dev				Address of device to draw to.
  * @param	string			Character string to draw.
  * @param	len				Length of string.
  * @param	x				X coordinate to draw at.
  * @param	page			Page to draw at.
  * @param	align			Alignment direction (SH1106_TEXT_ALIGN_LEFT or SH1106_TEXT_ALIGN_RIGHT).
  * @retval i2c_status_t	Status
  */
i2c_status_t sh1106_drawString_aligned(uint16_t dev, const char* string, size_t len, uint8_t x, uint8_t page, sh1106_text_align align){
	if(align == SH1106_TEXT_ALIGN_LEFT){
		return sh1106_drawString(dev, string, len, x, page);
	}else if(align == SH1106_TEXT_ALIGN_RIGHT){
		uint8_t alignedX = (SH1106_DISPLAY_WIDTH_PX - (x + (len * SH1106_FONT_WIDTH)));
		return sh1106_drawString(dev, string, len, alignedX, page);
	}

	return I2C_PARAM_ERR;
}

/**
  * @brief	Create the command buffer to set the current column to write to..
  * @param  col				Column to write to.
  * @param	buf				Output buffer
  * @param	offset			Whether to offset the column to visible display (true expects 0-128)
  * @retval void			Void
  */
void sh1106_setColAddressCommand(uint8_t col, uint8_t* buf, bool offset){
	if(offset){
		col += ((SH1106_DISPLAY_WIDTH_PX_FULL - SH1106_DISPLAY_WIDTH_PX) / 2);
		assert(col >= ((SH1106_DISPLAY_WIDTH_PX_FULL - SH1106_DISPLAY_WIDTH_PX) / 2)
				&& col < (SH1106_DISPLAY_WIDTH_PX_FULL - ((SH1106_DISPLAY_WIDTH_PX_FULL - SH1106_DISPLAY_WIDTH_PX) / 2))); //Ensure offset is within display visible bounds.
	}else{
		assert(col < SH1106_DISPLAY_WIDTH_PX_FULL); //Ensure offset is within display RAM bounds.
	}
	buf[0] = SH1106_COM_COL_ADDR_LOWER_SET(col & 0xF);
	buf[1] = SH1106_COM_COL_ADDR_HIGHER_SET(col >> 4);
}
