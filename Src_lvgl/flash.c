/*
 * flash.c
 *
 *  Created on: Oct 3, 2023
 *      Author: O.Onopriienko
 */
#include "flash.h"
#include "w25qxx.h"

#define BUFFER_SIZE 4
uint8_t buffer[BUFFER_SIZE];


void read_settings_from_flash() {
	W25qxx_ReadSector(buffer, 1, 0, BUFFER_SIZE);
	HAL_Delay(50);
}
void write_settings_to_flash() {
	W25qxx_EraseSector(1);
	HAL_Delay(50);
	W25qxx_WriteSector(buffer, 1, 0, BUFFER_SIZE);
	HAL_Delay(50);
}
uint8_t read_setting_brightness(){
	return buffer[INDEX_BRIGHTNESS_PERCENTAGE];
}
uint8_t read_setting_volume(){
	return buffer[INDEX_VOLUME_PERCENTAGE];
}
uint8_t read_setting_vibration(){
	return buffer[INDEX_VIBRATION_PERCENTAGE];
}
uint8_t read_setting_standby(){
	return buffer[INDEX_STANDBY_INSEC];
}
void write_setting_brightness(uint8_t value){
	buffer[INDEX_BRIGHTNESS_PERCENTAGE]= value;
}
void write_setting_volume(uint8_t value){
	buffer[INDEX_VOLUME_PERCENTAGE]=value;
}
void write_setting_vibration(uint8_t value){
	buffer[INDEX_VIBRATION_PERCENTAGE] = value;
}
void write_setting_standby(uint8_t value){
	buffer[INDEX_STANDBY_INSEC] = value;
}
