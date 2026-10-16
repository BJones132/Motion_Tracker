/*
 * sh1106.h
 *
 *  Created on: 1 Oct 2026
 *      Author: Billy Jones
 */

#ifndef CUSTOM_SH1106_I2C_INC_SH1106_H_
#define CUSTOM_SH1106_I2C_INC_SH1106_H_

#include <stdlib.h>

#include "sh1106_config.h"
#include "sh1106_opcodes.h"
#include "sh1106_fonts.h"

#include "i2c_bus.h"
#include "i2c_types.h"

i2c_status_t sh1106_init(uint16_t dev);

i2c_status_t sh1106_clear(uint16_t dev);

i2c_status_t sh1106_drawString(uint16_t dev, const char* string, size_t len, uint8_t x, uint8_t page);
i2c_status_t sh1106_drawGlyph(uint16_t dev, const char glyph, uint8_t x, uint8_t page);

typedef enum{
	SH1106_TEXT_ALIGN_LEFT = 0, SH1106_TEXT_ALIGN_RIGHT = 1
} sh1106_text_align;
i2c_status_t sh1106_drawString_aligned(uint16_t dev, const char* string, size_t len, uint8_t x, uint8_t page, sh1106_text_align align);

void sh1106_setColAddressCommand(uint8_t col, uint8_t* buf, bool offset);

#endif /* CUSTOM_SH1106_I2C_INC_SH1106_H_ */
