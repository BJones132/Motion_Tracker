/*
 * mt_ui.c
 *
 *  Created on: 13 Oct 2026
 *      Author: Billy Jones
 */

#include "mt_ui.h"

uint16_t mt_ui_dev;

/**
  * @brief	(LOCAL) Display the setting up screen to the user.
  * @retval void
  */
static void show_setup_screen(){
	if(mt_ui_dev == 0)
		return;
	const char setupString[] = "Setting Up...";
	const char dontMoveString[] = "Don't Move Device";
	sh1106_drawString(mt_ui_dev, setupString, sizeof(setupString), 25, 4);
	sh1106_drawString(mt_ui_dev, dontMoveString, sizeof(dontMoveString), 18,2);
}

/**
  * @brief	Draw an aligned/justified string of characters to display.
  * @param  dev			Address of device to draw to.
  * @retval mt_status_t	Status
  */
mt_status_t mt_ui_init(uint16_t dev){
	if(dev == 0)
		return MT_PARAM_ERR;
	mt_ui_dev = dev;

	sh1106_init(dev);
	show_setup_screen();

	return MT_OK;
}


