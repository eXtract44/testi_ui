#ifndef INC_HARDWARE_H_
#define INC_HARDWARE_H_
#include <stdint.h>
#include <main.h>
#include <stdbool.h>

void delay_mili(uint32_t __IO mili);
void turn_on_buttons();
void turn_off_buttons();
void start_buzzer(uint16_t time, uint16_t freq);
void start_vibration(uint16_t time);
void run_test_led(uint8_t status);
void turn_on_24v_internal();
void turn_off_24v_internal();
void turn_on_24v_external();
void turn_off_24v_external();
void toggle_24v_external();
void toggle_24v_internal();

#endif /* INC_HARDWARE_H_ */
