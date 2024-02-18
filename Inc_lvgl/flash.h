/*
 * flash.h
 *
 *  Created on: Oct 3, 2023
 *      Author: Alex
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include <stdint.h>

enum flash_index{
	INDEX_BRIGHTNESS_PERCENTAGE,
	INDEX_VOLUME_PERCENTAGE,
	INDEX_VIBRATION_PERCENTAGE,
	INDEX_STANDBY_INSEC,
};

void read_settings_from_flash();
void write_settings_to_flash();


uint8_t read_setting_brightness();
uint8_t read_setting_volume();
uint8_t read_setting_vibration();
uint8_t read_setting_standby();


void write_setting_brightness(uint8_t value);
void write_setting_volume(uint8_t value);
void write_setting_vibration(uint8_t value);
void write_setting_standby(uint8_t value);

#endif /* INC_FLASH_H_ */
