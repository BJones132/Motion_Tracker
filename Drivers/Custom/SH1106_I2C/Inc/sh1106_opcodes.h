/*
 * sh1106_opcodes.h
 *
 *  Created on: 10 Oct 2026
 *      Author: Billy Jones
 */

#ifndef CUSTOM_SH1106_I2C_INC_SH1106_OPCODES_H_
#define CUSTOM_SH1106_I2C_INC_SH1106_OPCODES_H_

#define SH1106_COMMAND_NON_CONT					0x80U
#define SH1106_COMMAND_CONT						0x00U

#define SH1106_DATA_NON_CONT					0xC0U
#define SH1106_DATA_CONT						0x40U

//Default
#define SH1106_COM_ADC_REMAP_LEFT				0xA0U
#define SH1106_COM_ADC_REMAP_RIGHT				0xA1U

#define SH1106_COM_COMPADS_MODE_SET				0xDAU
//Default
#define SH1106_DAT_COMPADS_MODE_ALT				0x12U
#define SH1106_DAT_COMPADS_MODE_SEQ				0x02U

//Used to vertically flip the output
//Default
#define SH1106_COM_OUTPUT_SCAN_DIR_STD			0xC0U
#define SH1106_COM_OUTPUT_SCAN_DIR_INV			0xC8U

#define SH1106_COM_INVERT_DISPLAY_ON			0xA7
#define SH1106_COM_INVERT_DISPLAY_OFF			0xA6

//Set multiplex ration
#define SH1106_COM_MULTI_RATION_SET				0xA8U
//Default (63/Max ratio)
#define SH1106_DAT_MULTI_RATION_DEFAULT			0x3FU

#define SH1106_COM_CLOCK_SET					0xD5U
#define SH1106_DAT_CLOCK_DEFAULT				0x50U

#define SH1106_COM_VCOM_DSEL_LEVEL_SET			0xDBU
#define SH1106_DAT_VCOM_DSEL_LEVEL_DEFAULT		0x35U

//Set contrast of display (0-FF/255)
#define SH1106_COM_CONTRAST_SET					0x81U
#define SH1106_DAT_CONTRAST_DEFAULT				0x80U

#define SH1106_COM_PUMP_VOLT_DEFAULT			0x32U

#define SH1106_COM_DC_PUMP_SET					0xADU
//Default (Used when display is only powered with 3V3)
#define SH1106_DAT_DC_PUMP_ON					0x8BU
//Used when supplied with external pump voltage
#define SH1106_DAT_DC_PUMP_OFF					0x8AU

#define SH1106_COM_RWM_START					0xE0U
#define SH1106_COM_RWM_END						0xEEU

#define SH1106_COM_DISPLAY_ON					0xAFU
#define SH1106_COM_DISPLAY_OFF					0xAEU

#define SH1106_COM_START_LINE_DEFAULT			0x32U

//Assertion should likely be used here to ensure only 8 pages can be accessed.
#define SH1106_COM_PAGE_ADDR_SET(addr)			(0xB0U + (addr))

//Assertion should likely be used here to ensure only one nibble is used in each.
#define SH1106_COM_COL_ADDR_LOWER_SET(addr)		(addr)
#define SH1106_COM_COL_ADDR_HIGHER_SET(addr)	(0x10U + (addr))

#endif /* CUSTOM_SH1106_I2C_INC_SH1106_OPCODES_H_ */
