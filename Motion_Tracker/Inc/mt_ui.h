/*
 * mt_ui.h
 *
 *  Created on: 13 Oct 2026
 *      Author: Billy Jones
 */

#ifndef INC_MT_UI_H_
#define INC_MT_UI_H_

#include <stdbool.h>

#include "mt_config.h"
#include "mt_types.h"
#include "sh1106.h"

mt_status_t mt_ui_init(uint16_t dev);

mt_status_t mt_ui_update(mt_imu_data* data, bool first_update);

#endif /* INC_MT_UI_H_ */
