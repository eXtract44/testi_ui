#include "main.h"
#include "ina219.h"
#include "hardware.h"
#include "menu.h"
#include "../lvgl/examples/lv_examples.h"
#include "../lvgl/lv_port_indev.h"

#include "spi_ili9341.h"
#include "adc.h"
#include "moving-median.h"
#include "flash.h"

//extern RTC_HandleTypeDef hrtc;
extern volatile uint16_t adc_data[];

extern lv_indev_t *indev_keypad; // for btn group

char str[20]; // buffer for chars

/*Prototyp*/
void draw_menu_main();
void draw_menu_sensor();
void draw_spinbox_measure_range1_2(bool status);
void draw_pt100_menu(bool status);
void draw_menu_actor();
void draw_menu_actor_switch(bool status);
void draw_menu_info();
void draw_menu_settings();
void draw_menu_settings_adc(bool status);
static lv_obj_t* create_button(lv_obj_t *parent, const char *text, lv_coord_t w, lv_coord_t h, lv_align_t align, lv_coord_t x_ofs,
		lv_coord_t y_ofs, void *cb_fun);

/*Global variables*/
uint16_t buttons_delay_mS = 200;
uint8_t current_menu_index = 0;
bool callback_measure_range, callback_actor, callback_actor_open, callback_actor_closed = 0;
int32_t measure_range_1_min, measure_range_1_max, measure_range_2_min, measure_range_2_max = 0;
uint8_t current_index_measure_range = 1;
uint32_t cnt_pt100_graphic, cnt_standby = 0;
uint8_t set_value_0_10v_output = 0;


static lv_style_t style[NUMBERS_OF_STYLES];
const lv_font_t *font[NUMBERS_OF_FONTS];
static lv_obj_t *lv_object[NUMBERS_OF_OBJECTS];

lv_group_t *GROUP_BUTTONS;
lv_chart_series_t *lv_chart_new_series_pt100;
lv_timer_t *lv_timer_standby, *lv_timer_main, *lv_timer_sensor, *lv_timer_pt100, *lv_timer_actor;



void print_value(const char *flags, float value, lv_obj_t *value_lv) {
	lv_label_set_text_fmt(value_lv, flags, value);
}
void print_value_bar(const char *flags, int32_t value, lv_obj_t *value_lv, lv_obj_t *bar_lv) {
	print_value(flags, value, value_lv);
	lv_bar_set_value(bar_lv, value, LV_ANIM_ON);
}
void check_values() {
	if (read_adc_in1() > OVERVOLTAGE_INPUT_IN1 || read_adc_in2() > OVERVOLTAGE_INPUT_IN2) { // 12 V max input Voltage
		start_buzzer(500, read_setting_volume());
	}
}
void standby_handler() {
	if (read_setting_standby() >= 1) {
		++cnt_standby;  //1 tick == 2 sec
		if (cnt_standby > read_setting_standby() * 30 / 8) { //1/8 time from read_setting_standby()
			if (read_setting_brightness() < 25) {
				ILI9341_SetBrightness(10);
			} else {
				ILI9341_SetBrightness(25);
			}
		} else {
			ILI9341_SetBrightness(read_setting_brightness());
		}
		if (cnt_standby > read_setting_standby() * 30 / 2) { //1/2 from read_setting_standby()
			ILI9341_SetBrightness(1);
		}
		if (cnt_standby > read_setting_standby() * 30) {
			ILI9341_SetBrightness(1);
			turn_off_24v_internal();
			turn_off_24v_external();
			set_y(0);
		}
	}
}

static void timer_0_cb(lv_timer_t *lv_timer_standby) {
	standby_handler();
	static uint16_t cnt_battery_low = 0;
	++cnt_battery_low;
	if (cnt_battery_low > 360) {
		if (read_adc_battery() < BATTERY_LOW_VOLTAGE + 1) {
			lv_label_set_text(lv_object[VALUE_BATTERY], "low");
			cnt_battery_low = 0;
		}
	}
}
static void timer_1_cb(lv_timer_t *lv_timer_main) {
	print_value_bar("%.f%%", read_adc_battery(), lv_object[VALUE_BATTERY], lv_object[BAR_BATTERY]);
	print_value_bar("%.f*c", read_adc_mcuTemp(), lv_object[VALUE_TEMPERATURE], lv_object[BAR_TEMPERATURE]);
}
static void timer_2_cb(lv_timer_t *lv_timer_sensor) {
	print_value("%.1fV", read_adc_in1(), lv_object[SENSOR_VALUE_INPUT_1]);
	float scaled_input1 = map_val(read_adc_in1(), MIN_INPUT_VOLTAGE,
	MAX_INPUT_VOLTAGE, measure_range_1_min, measure_range_1_max);
	print_value("%.1f", scaled_input1, lv_object[SENSOR_VALUE_INPUT_1_SCALED]);
	print_value("%.1fV", read_adc_in2(), lv_object[SENSOR_VALUE_INPUT_2]);
	float scaled_input2 = map_val(read_adc_in2(), MIN_INPUT_VOLTAGE,
	MAX_INPUT_VOLTAGE, measure_range_2_min, measure_range_2_max);
	print_value("%.1f", scaled_input2, lv_object[SENSOR_VALUE_INPUT_2_SCALED]);
	print_value("%.fV", ina219_ReadBusVoltage(), lv_object[SENSOR_VALUE_OUTPUT_24V]);
	print_value("%.fmA", current_ina219(), lv_object[SENSOR_VALUE_OUTPUT_mA]);
	check_values();
}
static void timer_3_cb(lv_timer_t *lv_timer_pt100) {
	++cnt_pt100_graphic;
	if (read_adc_pt100() < PT100_TEMP_MIN || read_adc_pt100() > PT100_TEMP_MAX) {
		lv_label_set_text(lv_object[SENSOR_PT100_VALUE], "no sensor");
	} else {
		print_value("%.1f*c", read_adc_pt100(), lv_object[SENSOR_PT100_VALUE]);
		if (cnt_pt100_graphic >= 60) {
			cnt_pt100_graphic = 0;
			lv_chart_set_next_value(lv_object[SENSOR_PT100_CHART], lv_chart_new_series_pt100, read_adc_pt100());
		}
	}
}
static void timer_4_cb(lv_timer_t *lv_timer_actor) {
	if (callback_actor == 1) {
		if (read_adc_in1() >= INPUT_ACTIVATE && callback_actor_open == 0) {
			callback_actor_open = 1;
			lv_object[ACTOR_INPUT1_OPEN] = create_button(lv_scr_act(), "OPEN", 120, 30, LV_ALIGN_CENTER, -70, 30, NULL);
		}
		if (read_adc_in1() <= INPUT_ACTIVATE && callback_actor_open == 1) {
			callback_actor_open = 0;
			lv_obj_del_async(lv_object[ACTOR_INPUT1_OPEN]);
		}
		if (read_adc_in2() >= INPUT_ACTIVATE && callback_actor_closed == 0) {
			callback_actor_closed = 1;
			lv_object[ACTOR_INPUT2_CLOSED] = create_button(lv_scr_act(), "CLOSED", 120, 30, LV_ALIGN_CENTER, 70, 30, NULL);
		}
		if (read_adc_in2() <= INPUT_ACTIVATE && callback_actor_closed == 1) {
			callback_actor_closed = 0;
			lv_obj_del_async(lv_object[ACTOR_INPUT2_CLOSED]);
		}
	} else {
		print_value("%.1fV", read_adc_in1(), lv_object[ACTOR_VALUE_INPUT_1]);
		print_value("%.1fV", read_adc_y(), lv_object[ACTOR_VALUE_OUTPUT_0_10V]);
	}
	print_value("%.1fV", ina219_ReadBusVoltage(), lv_object[ACTOR_VALUE_OUTPUT_24V]);
	print_value("%.1fmA", current_ina219(), lv_object[ACTOR_VALUE_OUTPUT_mA]);
	check_values();
}
static void init_timers() {
	lv_timer_standby = lv_timer_create(timer_0_cb, 2000, NULL); //timer for standby_handler
	lv_timer_main = lv_timer_create(timer_1_cb, 1000, NULL); //timer for main menu
	lv_timer_pause(lv_timer_main);
	lv_timer_sensor = lv_timer_create(timer_2_cb, 1000, NULL); //timer for sensor menu
	lv_timer_pause(lv_timer_sensor);
	lv_timer_pt100 = lv_timer_create(timer_3_cb, 1000, NULL); //timer for pt100 menu
	lv_timer_pause(lv_timer_pt100);
	lv_timer_actor = lv_timer_create(timer_4_cb, 1000, NULL); //timer for actor menu
	lv_timer_pause(lv_timer_actor);
}
static void init_fonts() {
	font[FONT_SMALL] = &lv_font_montserrat_10;
	font[FONT_MEDIUM] = &lv_font_montserrat_18;
	font[FONT_LARGE] = &lv_font_montserrat_26;
}
static void init_style_bar() {
	lv_style_init(&style[STYLE_BAR_BACKGROUND]);
	lv_style_set_bg_opa(&style[STYLE_BAR_BACKGROUND], LV_OPA_COVER);
	lv_style_set_bg_color(&style[STYLE_BAR_BACKGROUND], lv_palette_main(LV_PALETTE_GREY));
	lv_style_set_radius(&style[STYLE_BAR_BACKGROUND], 6);
	lv_style_init(&style[STYLE_BAR_BATTERY]);
	lv_style_set_bg_opa(&style[STYLE_BAR_BATTERY], LV_OPA_COVER);
	lv_style_set_bg_color(&style[STYLE_BAR_BATTERY], lv_palette_main(LV_PALETTE_GREEN));
	lv_style_set_bg_grad_dir(&style[STYLE_BAR_BATTERY], LV_GRAD_DIR_HOR);
	lv_style_set_bg_grad_color(&style[STYLE_BAR_BATTERY], lv_palette_main(LV_PALETTE_RED));
	lv_style_set_radius(&style[STYLE_BAR_BATTERY], 6);
	lv_style_set_text_font(&style[STYLE_BAR_BATTERY], font[FONT_SMALL]);
	lv_style_set_text_color(&style[STYLE_BAR_BATTERY], lv_color_white());
	lv_style_init(&style[STYLE_BAR_MCU_TEMPERATURE]);
	lv_style_set_bg_opa(&style[STYLE_BAR_MCU_TEMPERATURE], LV_OPA_COVER);
	lv_style_set_bg_color(&style[STYLE_BAR_MCU_TEMPERATURE], lv_palette_main(LV_PALETTE_BLUE));
	lv_style_set_bg_grad_color(&style[STYLE_BAR_MCU_TEMPERATURE], lv_palette_main(LV_PALETTE_RED));
	lv_style_set_bg_grad_dir(&style[STYLE_BAR_MCU_TEMPERATURE], LV_GRAD_DIR_HOR);
	lv_style_set_radius(&style[STYLE_BAR_MCU_TEMPERATURE], 6);
	lv_style_set_text_font(&style[STYLE_BAR_MCU_TEMPERATURE], font[FONT_SMALL]);
	lv_style_set_text_color(&style[STYLE_BAR_MCU_TEMPERATURE], lv_color_white());
}
static void init_styles() {
	init_style_bar();
	lv_style_init(&style[STYLE_TEXT_SMALL]);
	lv_style_set_text_font(&style[STYLE_TEXT_SMALL], font[FONT_SMALL]);
	lv_style_init(&style[STYLE_TEXT_MEDIUM]);
	lv_style_set_text_font(&style[STYLE_TEXT_MEDIUM], font[FONT_MEDIUM]);
	lv_style_init(&style[STYLE_TEXT_LARGE]);
	lv_style_set_text_font(&style[STYLE_TEXT_LARGE], font[FONT_LARGE]);
}

static lv_obj_t* create_button(lv_obj_t *parent, const char *text, lv_coord_t w, lv_coord_t h, lv_align_t align, lv_coord_t x_ofs,
		lv_coord_t y_ofs, void *cb_fun) {
	lv_obj_t *cont = lv_btn_create(parent);
	lv_obj_set_size(cont, w, h);
	lv_obj_align(cont, align, x_ofs, y_ofs);
	lv_obj_add_event_cb(cont, cb_fun, LV_EVENT_ALL,
	NULL);
	lv_obj_t *label_for_button = lv_label_create(cont);
	lv_label_set_text(label_for_button, text);
	lv_obj_center(label_for_button);
	return cont;
}
static lv_obj_t* create_bar(lv_obj_t *bg, lv_coord_t bar_w, lv_coord_t bar_h, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs,
		int32_t range_min, int32_t range_max) {
	lv_obj_t *cont = lv_bar_create(bg);
	lv_obj_remove_style_all(cont);
	lv_obj_set_size(cont, bar_w, bar_h);
	lv_obj_align(cont, align, x_ofs, y_ofs);
	lv_bar_set_range(cont, range_min, range_max);
	lv_bar_set_value(cont, range_max, LV_ANIM_ON);
	return cont;
}
static void add_group_buttons(lv_obj_t **buttons, const uint16_t start_from_object, const uint8_t length) {
	if (length == 0) {
		return;
	}
	for (uint8_t i = start_from_object; i < start_from_object + length; i++) {
		lv_group_add_obj(GROUP_BUTTONS, buttons[i]);
	}
}
static void create_text(const char *text, lv_obj_t *bg, uint8_t theme, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs) {
	lv_obj_t *cont = lv_label_create(bg);
	lv_obj_remove_style_all(cont);
	switch (theme) {
	case THEME_TEXT_SMALL:
		lv_obj_add_style(cont, &style[STYLE_TEXT_SMALL], 0);
		break;
	case THEME_TEXT_MEDIUM:
		lv_obj_add_style(cont, &style[STYLE_TEXT_MEDIUM], 0);
		break;
	case THEME_TEXT_LARGE:
		lv_obj_add_style(cont, &style[STYLE_TEXT_LARGE], 0);
		break;
	}
	lv_label_set_text(cont, text);
	lv_obj_align(cont, align, x_ofs, y_ofs);
}
static void create_background_init(lv_obj_t *parent, lv_coord_t w, lv_coord_t h, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs) {
	lv_obj_t *cont = lv_obj_create(parent);
	lv_obj_set_size(cont, w, h);
	lv_obj_align(cont, align, x_ofs, y_ofs);
}
static lv_obj_t* create_background(lv_obj_t *bg, lv_coord_t w, lv_coord_t h, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs) {
	lv_obj_t *cont = lv_obj_create(bg);
	lv_obj_set_size(cont, w, h);
	lv_obj_align(cont, align, x_ofs, y_ofs);
	return cont;
}
static void create_conn(lv_obj_t *bg, const char *text, lv_coord_t w, lv_coord_t h, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs) {
	lv_obj_t *cont = lv_btn_create(bg);
	lv_obj_set_size(cont, w, h);
	lv_obj_align(cont, align, x_ofs, y_ofs);
	lv_obj_t *label_for_conn = lv_label_create(cont);
	lv_label_set_text(label_for_conn, text);
	lv_obj_center(label_for_conn);
}
static void create_connector(lv_obj_t *bg, uint8_t screen, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs) {
	const uint8_t w_connector = 36;
	const uint8_t h_connector = 28;
	switch (screen) {
	case SENSOR_SCREEN:
		create_conn(bg, "P+", w_connector, h_connector, align, x_ofs, y_ofs);
		create_conn(bg, "P-", w_connector, h_connector, align, x_ofs + w_connector + 2, y_ofs);
		create_conn(bg, "GN", w_connector, h_connector, align, x_ofs + w_connector * 2 + 4, y_ofs);
		create_conn(bg, "I2", w_connector, h_connector, align, x_ofs + w_connector * 3 + 6, y_ofs);
		create_conn(bg, "I1", w_connector, h_connector, align, x_ofs + w_connector * 4 + 8, y_ofs);
		create_conn(bg, "  ", w_connector, h_connector, align, x_ofs + w_connector * 5 + 10, y_ofs);
		create_conn(bg, "GN", w_connector, h_connector, align, x_ofs + w_connector * 6 + 12, y_ofs);
		create_conn(bg, "24", w_connector, h_connector, align, x_ofs + w_connector * 7 + 14, y_ofs);
		break;
	case ACTOR_SCREEN:
		create_conn(bg, "  ", w_connector, h_connector, align, x_ofs, y_ofs);
		create_conn(bg, "  ", w_connector, h_connector, align, x_ofs + w_connector + 2, y_ofs);
		create_conn(bg, "  ", w_connector, h_connector, align, x_ofs + w_connector * 2 + 4, y_ofs);
		create_conn(bg, "I2", w_connector, h_connector, align, x_ofs + w_connector * 3 + 6, y_ofs);
		create_conn(bg, "I1", w_connector, h_connector, align, x_ofs + w_connector * 4 + 8, y_ofs);
		create_conn(bg, "OU", w_connector, h_connector, align, x_ofs + w_connector * 5 + 10, y_ofs);
		create_conn(bg, "GN", w_connector, h_connector, align, x_ofs + w_connector * 6 + 12, y_ofs);
		create_conn(bg, "24", w_connector, h_connector, align, x_ofs + w_connector * 7 + 14, y_ofs);
		break;
	case INFO_SCREEN:
		create_conn(bg, "P+", w_connector, h_connector, align, x_ofs, y_ofs);
		create_conn(bg, "P-", w_connector, h_connector, align, x_ofs + w_connector + 2, y_ofs);
		create_conn(bg, "GN", w_connector, h_connector, align, x_ofs + w_connector * 2 + 4, y_ofs);
		create_conn(bg, "I2", w_connector, h_connector, align, x_ofs + w_connector * 3 + 6, y_ofs);
		create_conn(bg, "I1", w_connector, h_connector, align, x_ofs + w_connector * 4 + 8, y_ofs);
		create_conn(bg, "OU", w_connector, h_connector, align, x_ofs + w_connector * 5 + 10, y_ofs);
		create_conn(bg, "GN", w_connector, h_connector, align, x_ofs + w_connector * 6 + 12, y_ofs);
		create_conn(bg, "24", w_connector, h_connector, align, x_ofs + w_connector * 7 + 14, y_ofs);
		break;
	}
}
static lv_obj_t* create_label(lv_obj_t *screen, const char *text, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs) {
	lv_obj_t *con = lv_label_create(screen);
	lv_obj_align(con, align, x_ofs, y_ofs);
	lv_label_set_text(con, text);
	return con;
}
static void lv_obj_del_async_some(lv_obj_t **obj, const uint16_t start_from_object, const uint8_t length) {
	if (length == 0) {
		return;
	}
	for (uint8_t i = start_from_object; i < start_from_object + length; i++) {
		lv_obj_del_async(obj[i]);
	}
}
static lv_obj_t* create_chart(lv_obj_t *bg, lv_coord_t chart_w, lv_coord_t chart_h, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs) {
	const int8_t chart_value_min = -10;
	const uint8_t chart_value_max = 100;
	lv_obj_t *chart = lv_chart_create(bg);
	lv_obj_set_size(chart, chart_w, chart_h);
	lv_obj_align(chart, align, x_ofs, y_ofs);
	lv_chart_set_div_line_count(chart, 12, 10);
	lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, chart_value_min, chart_value_max);
	lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 5, 3, 12, 2,
	true, 40);
	lv_chart_new_series_pt100 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BROWN), LV_CHART_AXIS_PRIMARY_Y);
	return chart;
}
static lv_obj_t* create_spinbox(lv_obj_t *screen, lv_coord_t w, int32_t range_min, int32_t range_max, uint8_t digit_count, lv_align_t align,
		lv_coord_t x_ofs, lv_coord_t y_ofs) {
	lv_obj_t *spinbox = lv_spinbox_create(screen);
	lv_obj_set_width(spinbox, w);
	lv_spinbox_set_range(spinbox, range_min, range_max);
	lv_spinbox_set_value(spinbox, 0);
	lv_spinbox_set_digit_format(spinbox, digit_count, 0);
	lv_obj_align(spinbox, align, x_ofs, y_ofs);
	return spinbox;
}
static lv_obj_t* create_slider(lv_obj_t *bg, lv_coord_t w, lv_coord_t h, int32_t range_min, int32_t range_max, lv_align_t align,
		lv_coord_t x_ofs, lv_coord_t y_ofs, void *cb_fun) {
	lv_obj_t *slider = lv_slider_create(bg);
	lv_slider_set_range(slider, range_min, range_max);
	lv_obj_align(slider, align, x_ofs, y_ofs);
	lv_obj_set_size(slider, w, h);
	lv_obj_add_event_cb(slider, cb_fun, LV_EVENT_VALUE_CHANGED, NULL);
	return slider;
}
void create_battery_bar() {
	lv_object[BAR_BATTERY] = create_bar(lv_object[MAIN_SCREEN], 50, 20, LV_ALIGN_TOP_RIGHT, -10, 20, 0, 100);
	lv_obj_add_style(lv_object[BAR_BATTERY], &style[STYLE_BAR_BACKGROUND], 0);
	lv_obj_add_style(lv_object[BAR_BATTERY], &style[STYLE_BAR_BATTERY], LV_PART_INDICATOR);
	lv_object[VALUE_BATTERY] = lv_label_create(lv_object[BAR_BATTERY]);
	lv_obj_center(lv_object[VALUE_BATTERY]);
}
void create_temperature_bar() {
	lv_object[BAR_TEMPERATURE] = create_bar(lv_object[MAIN_SCREEN], 50, 20, LV_ALIGN_TOP_LEFT, 10, 20, 0, 50);
	lv_obj_add_style(lv_object[BAR_TEMPERATURE], &style[STYLE_BAR_BACKGROUND], 0);
	lv_obj_add_style(lv_object[BAR_TEMPERATURE], &style[STYLE_BAR_MCU_TEMPERATURE], LV_PART_INDICATOR);
	lv_object[VALUE_TEMPERATURE] = lv_label_create(lv_object[BAR_TEMPERATURE]);
	lv_obj_center(lv_object[VALUE_TEMPERATURE]);
}

static void event_handler_btn_1_main(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		draw_menu_sensor();
	}
}
static void event_handler_btn_2_main(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		draw_menu_actor();
	}
}
static void event_handler_btn_3_main(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		read_settings_from_flash();
		draw_menu_settings();
	}
}
static void event_handler_btn_4_main(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		draw_menu_info();
	}
}
static void event_handler_btn_1_sensor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		callback_measure_range = 0;
		draw_spinbox_measure_range1_2(0);
		current_index_measure_range = 1;
	}
}
static void event_handler_btn_2_sensor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		callback_measure_range = 1;
		draw_spinbox_measure_range1_2(0);
	}
}
static void event_handler_btn_3_sensor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		toggle_24v_external();
	}
}
static void event_handler_btn_4_sensor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		turn_off_24v_external();
		draw_menu_main();
	}
}
static void event_handler_btn_5_sensor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		draw_pt100_menu(0);
		turn_on_24v_internal();
		cnt_pt100_graphic = 0;
	}
}
static void event_handler_spinbox_1_btn_1_sensor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_LONG_PRESSED) {
		start_buzzer(50, read_setting_volume());
		lv_spinbox_step_next(lv_object[SENSOR_SPINBOX_1]);
	}
	if (code == LV_EVENT_SHORT_CLICKED) {
		start_buzzer(50, read_setting_volume());
		lv_spinbox_decrement(lv_object[SENSOR_SPINBOX_1]);
	}
}
static void event_handler_spinbox_1_btn_2_sensor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_LONG_PRESSED) {
		start_buzzer(50, read_setting_volume());
		lv_spinbox_step_prev(lv_object[SENSOR_SPINBOX_1]);
	}
	if (code == LV_EVENT_SHORT_CLICKED) {
		start_buzzer(50, read_setting_volume());
		lv_spinbox_increment(lv_object[SENSOR_SPINBOX_1]);
	}
}
static void event_handler_spinbox_2_btn_1_sensor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_LONG_PRESSED) {
		start_buzzer(50, read_setting_volume());
		lv_spinbox_step_next(lv_object[SENSOR_SPINBOX_2]);
	}
	if (code == LV_EVENT_SHORT_CLICKED) {
		start_buzzer(50, read_setting_volume());
		lv_spinbox_decrement(lv_object[SENSOR_SPINBOX_2]);
	}
}
static void event_handler_spinbox_2_btn_2_sensor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_LONG_PRESSED) {
		start_buzzer(50, read_setting_volume());
		lv_spinbox_step_prev(lv_object[SENSOR_SPINBOX_2]);
	}
	if (code == LV_EVENT_SHORT_CLICKED) {
		start_buzzer(50, read_setting_volume());
		lv_spinbox_increment(lv_object[SENSOR_SPINBOX_2]);
	}
}
static void event_handler_spinbox_1_2_btn_1_sensor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_buzzer(50, read_setting_volume());
		if (callback_measure_range == 0) {
			measure_range_1_min = lv_spinbox_get_value(lv_object[SENSOR_SPINBOX_1]);
			measure_range_1_max = lv_spinbox_get_value(lv_object[SENSOR_SPINBOX_2]);
			sprintf(str, "%d/%d", (int) measure_range_1_min, (int) measure_range_1_max);
			lv_label_set_text(lv_object[SENSOR_VALUE_MIN_MAX_MEASURE_RANGE_1], str);
			draw_spinbox_measure_range1_2(1);
		} else if (callback_measure_range == 1) {
			measure_range_2_min = lv_spinbox_get_value(lv_object[SENSOR_SPINBOX_1]);
			measure_range_2_max = lv_spinbox_get_value(lv_object[SENSOR_SPINBOX_2]);
			sprintf(str, "%d/%d", (int) measure_range_2_min, (int) measure_range_2_max);
			lv_label_set_text(lv_object[SENSOR_VALUE_MIN_MAX_MEASURE_RANGE_2], str);
			draw_spinbox_measure_range1_2(1);
		}
		current_index_measure_range = 1;
		current_menu_index = CURRENT_MENU_SENSOR;
	}
}
static void event_handler_chart_pt100_btn_1_sensor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_buzzer(50, read_setting_volume());
		draw_pt100_menu(1);
		turn_off_24v_internal();
	}
}
static void event_handler_btn_1_actor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_2) {
		set_value_0_10v_output++;
		if (set_value_0_10v_output > 12) {
			set_value_0_10v_output = 12;
		}
		set_y(set_value_0_10v_output);
		start_buzzer(50, read_setting_volume());
	}
	if (code == button_selection_method_1) {
		set_value_0_10v_output += 2;
		if (set_value_0_10v_output > 12) {
			set_value_0_10v_output = 12;
		}
		set_y(set_value_0_10v_output);
		start_buzzer(50, read_setting_volume());
	}
}
static void event_handler_btn_2_actor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_2) {
		set_value_0_10v_output--;
		if (set_value_0_10v_output <= 0 || set_value_0_10v_output >= 13) {
			set_value_0_10v_output = 0;
		}
		set_y(set_value_0_10v_output);
		start_buzzer(50, read_setting_volume());
	}
	if (code == button_selection_method_1) {
		set_value_0_10v_output -= 2;
		if (set_value_0_10v_output <= 0 || set_value_0_10v_output >= 13) {
			set_value_0_10v_output = 0;
		}
		set_y(set_value_0_10v_output);
		start_buzzer(50, read_setting_volume());
	}
}
static void event_handler_btn_3_actor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		toggle_24v_external();
		toggle_24v_internal();
	}
}
static void event_handler_btn_4_actor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		turn_off_24v_external();
		turn_off_24v_internal();
		set_y(0);
		draw_menu_main();
	}
}
static void event_handler_btn_5_actor(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		callback_actor = 1;
		set_y(10);
		draw_menu_actor_switch(0);
	}
}
static void event_handler_btn_1_actor_switch(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		toggle_24v_external();
		toggle_24v_internal();
	}
}
static void event_handler_btn_2_actor_switch(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		callback_actor = 0;
		turn_off_24v_external();
		turn_off_24v_internal();
		draw_menu_actor_switch(1);
	}
}
static void event_handler_slider_1_settings(lv_event_t *e) {
	lv_object[SETTINGS_SCREEN_SLIDER_BRIGHTNESS] = lv_event_get_target(e);
	sprintf(str, "%d%%", (uint8_t) lv_slider_get_value(lv_object[SETTINGS_SCREEN_SLIDER_BRIGHTNESS]));
	lv_label_set_text(lv_object[SETTINGS_SCREEN_SLIDER_BRIGHTNESS_VALUE], str);
	write_setting_brightness(lv_slider_get_value(lv_object[SETTINGS_SCREEN_SLIDER_BRIGHTNESS]));
	ILI9341_SetBrightness(lv_slider_get_value(lv_object[SETTINGS_SCREEN_SLIDER_BRIGHTNESS]));
}
static void event_handler_slider_2_settings(lv_event_t *e) {
	lv_object[SETTINGS_SCREEN_SLIDER_VOLUME] = lv_event_get_target(e);
	sprintf(str, "%d%%", (uint8_t) lv_slider_get_value(lv_object[SETTINGS_SCREEN_SLIDER_VOLUME]));
	lv_label_set_text(lv_object[SETTINGS_SCREEN_SLIDER_VOLUME_VALUE], str);
	write_setting_volume(lv_slider_get_value(lv_object[SETTINGS_SCREEN_SLIDER_VOLUME]));
	start_buzzer(50, lv_slider_get_value(lv_object[SETTINGS_SCREEN_SLIDER_VOLUME]));
}
static void event_handler_slider_3_settings(lv_event_t *e) {
	lv_object[SETTINGS_SCREEN_SLIDER_VIBRATION] = lv_event_get_target(e);
	sprintf(str, "%d%%", (uint8_t) lv_slider_get_value(lv_object[SETTINGS_SCREEN_SLIDER_VIBRATION]));
	lv_label_set_text(lv_object[SETTINGS_SCREEN_SLIDER_VIBRATION_VALUE], str);
	write_setting_vibration(lv_slider_get_value(lv_object[SETTINGS_SCREEN_SLIDER_VIBRATION]));
	start_vibration(lv_slider_get_value(lv_object[SETTINGS_SCREEN_SLIDER_VIBRATION]));
}
static void event_handler_slider_4_settings(lv_event_t *e) {
	lv_object[SETTINGS_SCREEN_SLIDER_STANDBY] = lv_event_get_target(e);
	sprintf(str, "%dmin", (uint8_t) lv_slider_get_value(lv_object[SETTINGS_SCREEN_SLIDER_STANDBY]));
	lv_label_set_text(lv_object[SETTINGS_SCREEN_SLIDER_STANDBY_VALUE], str);
	write_setting_standby(lv_slider_get_value(lv_object[SETTINGS_SCREEN_SLIDER_STANDBY]));
}
static void event_handler_btn_1_settings(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		write_settings_to_flash();
		draw_menu_main();
	}
}
static void event_handler_btn_1_info(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == button_selection_method_1) {
		start_vibration(read_setting_vibration());
		start_buzzer(50, read_setting_volume());
		draw_menu_main();
	}
}

static void create_menu_main() {
	para_t btn_1 = { .width = 120, .height = 50, .align = LV_ALIGN_CENTER, .x_ofs = -70, .y_ofs = -20 };
	para_t btn_2 = { .width = 120, .height = 50, .align = LV_ALIGN_CENTER, .x_ofs = 70, .y_ofs = -20 };
	para_t btn_3 = { .width = 120, .height = 50, .align = LV_ALIGN_CENTER, .x_ofs = -70, .y_ofs = 60 };
	para_t btn_4 = { .width = 120, .height = 50, .align = LV_ALIGN_CENTER, .x_ofs = 70, .y_ofs = 60 };
	lv_object[MAIN_SCREEN] = lv_obj_create(NULL);
	lv_object[MAIN_SCREEN_BACKGROUND] = lv_obj_create(lv_object[MAIN_SCREEN]);
	lv_obj_set_size(lv_object[MAIN_SCREEN_BACKGROUND], MY_DISP_HOR_RES,
	MY_DISP_VER_RES);
	lv_object[MAIN_SCREEN_BUTTON_1] = create_button(lv_object[MAIN_SCREEN], "SENSORS", btn_1.width, btn_1.height, btn_1.align, btn_1.x_ofs,
			btn_1.y_ofs, event_handler_btn_1_main);
	lv_object[MAIN_SCREEN_BUTTON_2] = create_button(lv_object[MAIN_SCREEN], "ACTORS", btn_2.width, btn_2.height, btn_2.align, btn_2.x_ofs,
			btn_2.y_ofs, event_handler_btn_2_main);
	lv_object[MAIN_SCREEN_BUTTON_3] = create_button(lv_object[MAIN_SCREEN], "SETTINGS", btn_3.width, btn_3.height, btn_3.align, btn_3.x_ofs,
			btn_3.y_ofs, event_handler_btn_3_main);
	lv_object[MAIN_SCREEN_BUTTON_4] = create_button(lv_object[MAIN_SCREEN], "INFO", btn_4.width, btn_4.height, btn_4.align, btn_4.x_ofs,
			btn_4.y_ofs, event_handler_btn_4_main);
	create_battery_bar();
	create_temperature_bar();
	GROUP_BUTTONS = lv_group_create();
	lv_indev_set_group(indev_keypad, GROUP_BUTTONS);
}
static void create_menu_sensor() {
	para_t connector = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 2, .y_ofs = 0 };
	para_t btn_1 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = -110, .y_ofs = -170 };
	para_t btn_2 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = -110, .y_ofs = -90 };
	para_t btn_3 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = -110, .y_ofs = -10 };
	para_t btn_4 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = 0, .y_ofs = -10 };
	para_t btn_5 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = 110, .y_ofs = -10 };
	para_t value_min_max_meas_ran_1 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 100, .y_ofs = 45 };
	para_t values_input_1_bg = { .width = 230, .height = 31, .align = LV_ALIGN_TOP_LEFT, .x_ofs = 0, .y_ofs = 81 };
	para_t text_input_1 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 20, .y_ofs = 86 };
	para_t value_input_1 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 95, .y_ofs = 81 };
	para_t value_input_1_scaled = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 165, .y_ofs = 86 };
	para_t value_min_max_meas_ran_2 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 100, .y_ofs = 125 };
	para_t values_input_2_bg = { .width = 230, .height = 31, .align = LV_ALIGN_TOP_LEFT, .x_ofs = 0, .y_ofs = 160 };
	para_t text_input_2 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 20, .y_ofs = 165 };
	para_t value_input_2 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 95, .y_ofs = 160 };
	para_t value_input_2_scaled = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 165, .y_ofs = 165 };
	para_t value_u_p_i_bg = { .width = 70, .height = 50, .align = LV_ALIGN_TOP_LEFT, .x_ofs = 230, .y_ofs = 30 };
	para_t value_u = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 245, .y_ofs = 35 };
	para_t value_i = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 231, .y_ofs = 55 };

	lv_object[SENSOR_SCREEN] = lv_obj_create(NULL);
	lv_object[SENSOR_SCREEN_BACKGROUND] = lv_obj_create(lv_object[SENSOR_SCREEN]);
	lv_obj_set_size(lv_object[SENSOR_SCREEN_BACKGROUND], MY_DISP_HOR_RES,
	MY_DISP_VER_RES);
	create_connector(lv_object[SENSOR_SCREEN], SENSOR_SCREEN, connector.align, connector.x_ofs, connector.y_ofs);
	create_background_init(lv_object[SENSOR_SCREEN], values_input_1_bg.width, values_input_1_bg.height, values_input_1_bg.align,
			values_input_1_bg.x_ofs, values_input_1_bg.y_ofs);
	create_background_init(lv_object[SENSOR_SCREEN], values_input_2_bg.width, values_input_2_bg.height, values_input_2_bg.align,
			values_input_2_bg.x_ofs, values_input_2_bg.y_ofs);
	create_background_init(lv_object[SENSOR_SCREEN], value_u_p_i_bg.width, value_u_p_i_bg.height, value_u_p_i_bg.align,
			value_u_p_i_bg.x_ofs, value_u_p_i_bg.y_ofs);
	lv_object[SENSOR_SCREEN_BUTTON_1] = create_button(lv_object[SENSOR_SCREEN], "MR1", btn_1.width, btn_1.height, btn_1.align, btn_1.x_ofs,
			btn_1.y_ofs, event_handler_btn_1_sensor);
	lv_object[SENSOR_SCREEN_BUTTON_2] = create_button(lv_object[SENSOR_SCREEN], "MR2", btn_2.width, btn_2.height, btn_2.align, btn_2.x_ofs,
			btn_2.y_ofs, event_handler_btn_2_sensor);
	lv_object[SENSOR_SCREEN_BUTTON_3] = create_button(lv_object[SENSOR_SCREEN], "24V", btn_3.width, btn_3.height, btn_3.align, btn_3.x_ofs,
			btn_3.y_ofs, event_handler_btn_3_sensor);
	lv_object[SENSOR_SCREEN_BUTTON_4] = create_button(lv_object[SENSOR_SCREEN], "HOME", btn_4.width, btn_4.height, btn_4.align, btn_4.x_ofs,
			btn_4.y_ofs, event_handler_btn_4_sensor);
	lv_object[SENSOR_SCREEN_BUTTON_5] = create_button(lv_object[SENSOR_SCREEN], "PT100", btn_5.width, btn_5.height, btn_5.align,
			btn_5.x_ofs, btn_5.y_ofs, event_handler_btn_5_sensor);
	create_text("IN1", lv_object[SENSOR_SCREEN], THEME_TEXT_MEDIUM, text_input_1.align, text_input_1.x_ofs, text_input_1.y_ofs);
	create_text("IN2", lv_object[SENSOR_SCREEN], THEME_TEXT_MEDIUM, text_input_2.align, text_input_2.x_ofs, text_input_2.y_ofs);
	lv_object[SENSOR_VALUE_MIN_MAX_MEASURE_RANGE_1] = create_label(lv_object[SENSOR_SCREEN], "0/0", value_min_max_meas_ran_1.align,
			value_min_max_meas_ran_1.x_ofs, value_min_max_meas_ran_1.y_ofs);
	lv_object[SENSOR_VALUE_INPUT_1] = create_label(lv_object[SENSOR_SCREEN], "0V", value_input_1.align, value_input_1.x_ofs,
			value_input_1.y_ofs);
	lv_obj_add_style(lv_object[SENSOR_VALUE_INPUT_1], &style[STYLE_TEXT_LARGE], 0);
	lv_object[SENSOR_VALUE_INPUT_1_SCALED] = create_label(lv_object[SENSOR_SCREEN], "0", value_input_1_scaled.align,
			value_input_1_scaled.x_ofs, value_input_1_scaled.y_ofs);
	lv_object[SENSOR_VALUE_MIN_MAX_MEASURE_RANGE_2] = create_label(lv_object[SENSOR_SCREEN], "0/0", value_min_max_meas_ran_2.align,
			value_min_max_meas_ran_2.x_ofs, value_min_max_meas_ran_2.y_ofs);
	lv_object[SENSOR_VALUE_INPUT_2] = create_label(lv_object[SENSOR_SCREEN], "0V", value_input_2.align, value_input_2.x_ofs,
			value_input_2.y_ofs);
	lv_obj_add_style(lv_object[SENSOR_VALUE_INPUT_2], &style[STYLE_TEXT_LARGE], 0);
	lv_object[SENSOR_VALUE_INPUT_2_SCALED] = create_label(lv_object[SENSOR_SCREEN], "0", value_input_2_scaled.align,
			value_input_2_scaled.x_ofs, value_input_2_scaled.y_ofs);
	lv_object[SENSOR_VALUE_OUTPUT_24V] = create_label(lv_object[SENSOR_SCREEN], "0V", value_u.align, value_u.x_ofs, value_u.y_ofs);
	lv_object[SENSOR_VALUE_OUTPUT_mA] = create_label(lv_object[SENSOR_SCREEN], "0mA", value_i.align, value_i.x_ofs, value_i.y_ofs);
	lv_obj_add_style(lv_object[SENSOR_VALUE_OUTPUT_24V], &style[STYLE_TEXT_MEDIUM], 0);
	lv_obj_add_style(lv_object[SENSOR_VALUE_OUTPUT_mA], &style[STYLE_TEXT_MEDIUM], 0);
}
static void create_menu_actor() {
	para_t connector = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 2, .y_ofs = 0 };
	para_t btn_1 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = -110, .y_ofs = -170 };
	para_t btn_2 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = -110, .y_ofs = -90 };
	para_t btn_3 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = -110, .y_ofs = -10 };
	para_t btn_4 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = 0, .y_ofs = -10 };
	para_t btn_5 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = 110, .y_ofs = -10 };
	para_t values_input_1_bg = { .width = 230, .height = 31, .align = LV_ALIGN_TOP_LEFT, .x_ofs = 0, .y_ofs = 81 };
	para_t text_input_1 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 20, .y_ofs = 86 };
	para_t value_input_1 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 95, .y_ofs = 81 };
	para_t values_input_2_bg = { .width = 230, .height = 31, .align = LV_ALIGN_TOP_LEFT, .x_ofs = 0, .y_ofs = 160 };
	para_t text_input_2 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 20, .y_ofs = 165 };
	para_t value_input_2 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 95, .y_ofs = 160 };
	para_t value_u_p_i_bg = { .width = 70, .height = 50, .align = LV_ALIGN_TOP_LEFT, .x_ofs = 230, .y_ofs = 30 };
	para_t value_u = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 245, .y_ofs = 35 };
	para_t value_i = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 231, .y_ofs = 55 };
	lv_object[ACTOR_SCREEN] = lv_obj_create(NULL);
	lv_object[ACTOR_SCREEN_BACKGROUND] = lv_obj_create(lv_object[ACTOR_SCREEN]);
	lv_obj_set_size(lv_object[ACTOR_SCREEN_BACKGROUND], MY_DISP_HOR_RES,
	MY_DISP_VER_RES);
	create_connector(lv_object[ACTOR_SCREEN], ACTOR_SCREEN, connector.align, connector.x_ofs, connector.y_ofs);
	create_background_init(lv_object[ACTOR_SCREEN], values_input_1_bg.width, values_input_1_bg.height, values_input_1_bg.align,
			values_input_1_bg.x_ofs, values_input_1_bg.y_ofs);
	create_background_init(lv_object[ACTOR_SCREEN], values_input_2_bg.width, values_input_2_bg.height, values_input_2_bg.align,
			values_input_2_bg.x_ofs, values_input_2_bg.y_ofs);
	create_background_init(lv_object[ACTOR_SCREEN], value_u_p_i_bg.width, value_u_p_i_bg.height, value_u_p_i_bg.align, value_u_p_i_bg.x_ofs,
			value_u_p_i_bg.y_ofs);
	lv_object[ACTOR_SCREEN_BUTTON_1] = create_button(lv_object[ACTOR_SCREEN], "Y+", btn_1.width, btn_1.height, btn_1.align, btn_1.x_ofs,
			btn_1.y_ofs, event_handler_btn_1_actor);
	lv_object[ACTOR_SCREEN_BUTTON_2] = create_button(lv_object[ACTOR_SCREEN], "Y-", btn_2.width, btn_2.height, btn_2.align, btn_2.x_ofs,
			btn_2.y_ofs, event_handler_btn_2_actor);
	lv_object[ACTOR_SCREEN_BUTTON_3] = create_button(lv_object[ACTOR_SCREEN], "24V", btn_3.width, btn_3.height, btn_3.align, btn_3.x_ofs,
			btn_3.y_ofs, event_handler_btn_3_actor);
	lv_object[ACTOR_SCREEN_BUTTON_4] = create_button(lv_object[ACTOR_SCREEN], "HOME", btn_4.width, btn_4.height, btn_4.align, btn_4.x_ofs,
			btn_4.y_ofs, event_handler_btn_4_actor);
	lv_object[ACTOR_SCREEN_BUTTON_5] = create_button(lv_object[ACTOR_SCREEN], "SWIT", btn_5.width, btn_5.height, btn_5.align, btn_5.x_ofs,
			btn_5.y_ofs, event_handler_btn_5_actor);
	create_text(" Y ", lv_object[ACTOR_SCREEN], THEME_TEXT_MEDIUM, text_input_1.align, text_input_1.x_ofs, text_input_1.y_ofs);
	create_text("IN1", lv_object[ACTOR_SCREEN], THEME_TEXT_MEDIUM, text_input_2.align, text_input_2.x_ofs, text_input_2.y_ofs);
	lv_object[ACTOR_VALUE_OUTPUT_0_10V] = create_label(lv_object[ACTOR_SCREEN], "0V", value_input_1.align, value_input_1.x_ofs,
			value_input_1.y_ofs);
	lv_obj_add_style(lv_object[ACTOR_VALUE_OUTPUT_0_10V], &style[STYLE_TEXT_LARGE], 0);
	lv_object[ACTOR_VALUE_INPUT_1] = create_label(lv_object[ACTOR_SCREEN], "0V", value_input_2.align, value_input_2.x_ofs,
			value_input_2.y_ofs);
	lv_obj_add_style(lv_object[ACTOR_VALUE_INPUT_1], &style[STYLE_TEXT_LARGE], 0);

	lv_object[ACTOR_VALUE_OUTPUT_24V] = create_label(lv_object[ACTOR_SCREEN], "0V", value_u.align, value_u.x_ofs, value_u.y_ofs);
	lv_object[ACTOR_VALUE_OUTPUT_mA] = create_label(lv_object[ACTOR_SCREEN], "0mA", value_i.align, value_i.x_ofs, value_i.y_ofs);
	lv_obj_add_style(lv_object[ACTOR_VALUE_OUTPUT_24V], &style[STYLE_TEXT_MEDIUM], 0);
	lv_obj_add_style(lv_object[ACTOR_VALUE_OUTPUT_mA], &style[STYLE_TEXT_MEDIUM], 0);
}
static void create_menu_settings() {
	para_t btn_1 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = 0, .y_ofs = -10 };
	para_t text_string_1 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 10, .y_ofs = 10 };
	para_t slider_1 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 140, .y_ofs = 15 };
	para_t value_1 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 240, .y_ofs = 10 };
	para_t text_string_2 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 10, .y_ofs = 40 };
	para_t slider_2 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 140, .y_ofs = 45 };
	para_t value_2 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 240, .y_ofs = 40 };
	para_t text_string_3 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 10, .y_ofs = 70 };
	para_t slider_3 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 140, .y_ofs = 75 };
	para_t value_3 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 240, .y_ofs = 70 };
	para_t text_string_4 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 10, .y_ofs = 100 };
	para_t slider_4 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 140, .y_ofs = 105 };
	para_t value_4 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 240, .y_ofs = 100 };
	lv_object[SETTINGS_SCREEN] = lv_obj_create(NULL);
	lv_object[SETTINGS_SCREEN_BACKGROUND] = lv_obj_create(lv_object[SETTINGS_SCREEN]);
	lv_obj_set_size(lv_object[SETTINGS_SCREEN_BACKGROUND], MY_DISP_HOR_RES,
	MY_DISP_VER_RES);
	lv_object[SETTINGS_SCREEN_BUTTON_1] = create_button(lv_object[SETTINGS_SCREEN], "HOME", btn_1.width, btn_1.height, btn_1.align,
			btn_1.x_ofs, btn_1.y_ofs, event_handler_btn_1_settings);
	create_text("brightness", lv_object[SETTINGS_SCREEN], THEME_TEXT_MEDIUM, text_string_1.align, text_string_1.x_ofs, text_string_1.y_ofs);
	lv_object[SETTINGS_SCREEN_SLIDER_BRIGHTNESS] = create_slider(lv_object[SETTINGS_SCREEN], 80, 10, 0, 100, slider_1.align, slider_1.x_ofs,
			slider_1.y_ofs, event_handler_slider_1_settings);
	lv_slider_set_value(lv_object[SETTINGS_SCREEN_SLIDER_BRIGHTNESS], read_setting_brightness(), LV_ANIM_OFF);
	lv_object[SETTINGS_SCREEN_SLIDER_BRIGHTNESS_VALUE] = create_label(lv_object[SETTINGS_SCREEN], " ", value_1.align, value_1.x_ofs,
			value_1.y_ofs);
	create_text("volume", lv_object[SETTINGS_SCREEN], THEME_TEXT_MEDIUM, text_string_2.align, text_string_2.x_ofs,
			text_string_2.y_ofs);
	lv_object[SETTINGS_SCREEN_SLIDER_VOLUME] = create_slider(lv_object[SETTINGS_SCREEN], 80, 10, 0, 100, slider_2.align, slider_2.x_ofs,
			slider_2.y_ofs, event_handler_slider_2_settings);
	lv_slider_set_value(lv_object[SETTINGS_SCREEN_SLIDER_BRIGHTNESS], read_setting_volume(), LV_ANIM_OFF);
	lv_object[SETTINGS_SCREEN_SLIDER_VOLUME_VALUE] = create_label(lv_object[SETTINGS_SCREEN], " ", value_2.align, value_2.x_ofs,
			value_2.y_ofs);
	create_text("vibration", lv_object[SETTINGS_SCREEN], THEME_TEXT_MEDIUM, text_string_3.align, text_string_3.x_ofs, text_string_3.y_ofs);
	lv_object[SETTINGS_SCREEN_SLIDER_VIBRATION] = create_slider(lv_object[SETTINGS_SCREEN], 80, 10, 0, 100, slider_3.align, slider_3.x_ofs,
			slider_3.y_ofs, event_handler_slider_3_settings);
	lv_slider_set_value(lv_object[SETTINGS_SCREEN_SLIDER_VIBRATION], read_setting_vibration(), LV_ANIM_OFF);
	lv_object[SETTINGS_SCREEN_SLIDER_VIBRATION_VALUE] = create_label(lv_object[SETTINGS_SCREEN], " ", value_3.align, value_3.x_ofs,
			value_3.y_ofs);
	create_text("standbytime", lv_object[SETTINGS_SCREEN], THEME_TEXT_MEDIUM, text_string_4.align, text_string_4.x_ofs,
			text_string_4.y_ofs);
	lv_object[SETTINGS_SCREEN_SLIDER_STANDBY] = create_slider(lv_object[SETTINGS_SCREEN], 80, 10, 0, 100, slider_4.align, slider_4.x_ofs,
			slider_4.y_ofs, event_handler_slider_4_settings);
	lv_slider_set_value(lv_object[SETTINGS_SCREEN_SLIDER_STANDBY], read_setting_standby(), LV_ANIM_OFF);
	lv_object[SETTINGS_SCREEN_SLIDER_STANDBY_VALUE] = create_label(lv_object[SETTINGS_SCREEN], " ", value_4.align, value_4.x_ofs,
			value_4.y_ofs);
}
static void create_menu_info() {
	para_t connector = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 2, .y_ofs = 0 };
	para_t text_1 = { .align = LV_ALIGN_TOP_LEFT, .x_ofs = 10, .y_ofs = 40 };
	para_t btn_1 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = 0, .y_ofs = -10 };
	lv_object[INFO_SCREEN] = lv_obj_create(NULL);
	lv_object[INFO_SCREEN_BACKGROUND] = lv_obj_create(lv_object[INFO_SCREEN]);
	lv_obj_set_size(lv_object[INFO_SCREEN_BACKGROUND], MY_DISP_HOR_RES,
	MY_DISP_VER_RES);
	lv_object[INFO_SCREEN_BUTTON_1] = create_button(lv_object[INFO_SCREEN], "HOME", btn_1.width, btn_1.height, btn_1.align, btn_1.x_ofs,
			btn_1.y_ofs, event_handler_btn_1_info);
	create_text("Max output power  24VDC 0.5A", lv_object[INFO_SCREEN], THEME_TEXT_MEDIUM, text_1.align, text_1.x_ofs, text_1.y_ofs);
	create_text("Max voltage input 1/2 24V", lv_object[INFO_SCREEN], THEME_TEXT_MEDIUM, text_1.align, text_1.x_ofs, text_1.y_ofs + 30);
	create_text("Max voltage output 12V", lv_object[INFO_SCREEN], THEME_TEXT_MEDIUM, text_1.align, text_1.x_ofs, text_1.y_ofs + 30 * 2);
	create_text("Charge 8,4 V 1A", lv_object[INFO_SCREEN], THEME_TEXT_MEDIUM, text_1.align, text_1.x_ofs, text_1.y_ofs + 30 * 3);
	create_connector(lv_object[INFO_SCREEN], INFO_SCREEN, connector.align, connector.x_ofs, connector.y_ofs);

}

void draw_menu_main() {
	lv_group_remove_all_objs(GROUP_BUTTONS);
	add_group_buttons(lv_object, MAIN_SCREEN_BUTTON_1, NUMBERS_OF_BUTTONS_MAIN);
	lv_timer_pause(lv_timer_sensor);
	lv_timer_pause(lv_timer_pt100);
	lv_timer_pause(lv_timer_actor);
	lv_timer_pause(lv_timer_settings_adc);
	lv_timer_resume(lv_timer_main);
	lv_scr_load(lv_object[MAIN_SCREEN]);
	current_menu_index = CURRENT_MENU_MAIN;
}
void draw_menu_sensor() {
	lv_group_remove_all_objs(GROUP_BUTTONS);
	add_group_buttons(lv_object, SENSOR_SCREEN_BUTTON_1,
	NUMBERS_OF_BUTTONS_SENSOR);
	lv_timer_pause(lv_timer_main);
	lv_timer_resume(lv_timer_sensor);
	lv_scr_load(lv_object[SENSOR_SCREEN]);
	current_menu_index = CURRENT_MENU_SENSOR;
}
void draw_pt100_menu(bool status) {
	para_t btn_1 = { .width = 35, .height = 35, .align = LV_ALIGN_BOTTOM_RIGHT, .x_ofs = -10, .y_ofs = -20 };
	para_t bg = { .width = 302, .height = 200, .align = LV_ALIGN_CENTER, .x_ofs = 0, .y_ofs = 10 };
	para_t chart = { .width = 225, .height = 170, .align = LV_ALIGN_CENTER, .x_ofs = 0, .y_ofs = 20 };
	para_t value_pt100 = { .align = LV_ALIGN_TOP_MID, .x_ofs = 0, .y_ofs = 65 };
	if (status == 1) {
		lv_group_remove_all_objs(GROUP_BUTTONS);
		add_group_buttons(lv_object, SENSOR_SCREEN_BUTTON_1,
		NUMBERS_OF_BUTTONS_SENSOR);
		lv_obj_del_async_some(lv_object, SENSOR_PT100_CHART,
		NUMBERS_OF_OBJECTS_PT100_SENSOR);
		lv_timer_pause(lv_timer_pt100);
		lv_timer_resume(lv_timer_sensor);
	} else {
		lv_object[SENSOR_PT100_CHART_BACKGROUND] = create_background(lv_scr_act(), bg.width, bg.height, bg.align, bg.x_ofs, bg.y_ofs);
		lv_object[SENSOR_PT100_CHART] = create_chart(lv_scr_act(), chart.width, chart.height, chart.align, chart.x_ofs, chart.y_ofs);
		lv_object[SENSOR_PT100_BUTTON_1] = create_button(lv_scr_act(), "ok", btn_1.width, btn_1.height, btn_1.align, btn_1.x_ofs,
				btn_1.y_ofs, event_handler_chart_pt100_btn_1_sensor);
		lv_object[SENSOR_PT100_VALUE] = create_label(lv_scr_act(), "LOADING", value_pt100.align, value_pt100.x_ofs, value_pt100.y_ofs);
		lv_group_remove_all_objs(GROUP_BUTTONS);
		lv_group_add_obj(GROUP_BUTTONS, lv_object[SENSOR_PT100_BUTTON_1]);
		lv_timer_pause(lv_timer_sensor);
		lv_timer_resume(lv_timer_pt100);
	}
}
void draw_spinbox_measure_range1_2(bool status) {
	para_t bg = { .width = 250, .height = 180, .align = LV_ALIGN_CENTER, .x_ofs = 0, .y_ofs = 0 };
	para_t spinbox_1 = { .width = 100, .align = LV_ALIGN_CENTER, .x_ofs = 0, .y_ofs = 6 };
	para_t text_1 = { .align = LV_ALIGN_CENTER, .x_ofs = 0, .y_ofs = 38 };
	para_t spinbox_2 = { .width = 100, .align = LV_ALIGN_CENTER, .x_ofs = 0, .y_ofs = -60 };
	para_t text_2 = { .align = LV_ALIGN_CENTER, .x_ofs = 0, .y_ofs = -27 };
	para_t btn_1 = { .width = 35, .height = 35, .align = LV_ALIGN_CENTER, .x_ofs = -70, .y_ofs = 6 };
	para_t btn_2 = { .width = 35, .height = 35, .align = LV_ALIGN_CENTER, .x_ofs = 70, .y_ofs = 6 };
	para_t btn_3 = { .width = 35, .height = 35, .align = LV_ALIGN_CENTER, .x_ofs = -70, .y_ofs = -60 };
	para_t btn_4 = { .width = 35, .height = 35, .align = LV_ALIGN_CENTER, .x_ofs = 70, .y_ofs = -60 };
	para_t btn_5 = { .width = 35, .height = 35, .align = LV_ALIGN_CENTER, .x_ofs = 0, .y_ofs = 65 };
	if (status == 1) {
		lv_group_remove_all_objs(GROUP_BUTTONS);
		lv_obj_del_async_some(lv_object, SENSOR_SPINBOX_1,
		NUMBERS_OF_OBJECTS_SPINBOX);
		add_group_buttons(lv_object, SENSOR_SCREEN_BUTTON_1,
		NUMBERS_OF_BUTTONS_SENSOR);
	} else {
		lv_group_remove_all_objs(GROUP_BUTTONS);
		lv_object[SENSOR_SPINBOX_BACKGROUND] = create_background(lv_scr_act(), bg.width, bg.height, bg.align, bg.x_ofs, bg.y_ofs);
		lv_object[SENSOR_SPINBOX_1] = create_spinbox(lv_scr_act(), spinbox_1.width, -2500, 2500, 4, spinbox_1.align, spinbox_1.x_ofs,
				spinbox_1.y_ofs);
		lv_object[SENSOR_SPINBOX_2] = create_spinbox(lv_scr_act(), spinbox_2.width, 0, 50000, 4, spinbox_2.align, spinbox_2.x_ofs,
				spinbox_2.y_ofs);
		create_text("measure range min", lv_object[SENSOR_SPINBOX_BACKGROUND], THEME_TEXT_MEDIUM, text_1.align, text_1.x_ofs, text_1.y_ofs);
		create_text("measure range max", lv_object[SENSOR_SPINBOX_BACKGROUND], THEME_TEXT_MEDIUM, text_2.align, text_2.x_ofs, text_2.y_ofs);
		lv_object[SENSOR_SPINBOX_1_BUTTON_1] = create_button(lv_scr_act(), "-", btn_1.width, btn_1.height, btn_1.align, btn_1.x_ofs,
				btn_1.y_ofs, event_handler_spinbox_1_btn_1_sensor);
		lv_object[SENSOR_SPINBOX_1_BUTTON_2] = create_button(lv_scr_act(), "+", btn_2.width, btn_2.height, btn_2.align, btn_2.x_ofs,
				btn_2.y_ofs, event_handler_spinbox_1_btn_2_sensor);
		lv_object[SENSOR_SPINBOX_2_BUTTON_1] = create_button(lv_scr_act(), "-", btn_3.width, btn_3.height, btn_3.align, btn_3.x_ofs,
				btn_3.y_ofs, event_handler_spinbox_2_btn_1_sensor);
		lv_object[SENSOR_SPINBOX_2_BUTTON_2] = create_button(lv_scr_act(), "+", btn_4.width, btn_4.height, btn_4.align, btn_4.x_ofs,
				btn_4.y_ofs, event_handler_spinbox_2_btn_2_sensor);
		lv_object[SENSOR_SPINBOX_1_2_BUTTON1] = create_button(lv_scr_act(), "ok", btn_5.width, btn_5.height, btn_5.align, btn_5.x_ofs,
				btn_5.y_ofs, event_handler_spinbox_1_2_btn_1_sensor);
		add_group_buttons(lv_object, SENSOR_SPINBOX_1_BUTTON_1,
		NUMBERS_OF_BUTTONS_SENSOR_SPINBOX);
		current_menu_index = CURRENT_MENU_SENSOR_MEAS_RANGE_1;
	}
}
void draw_menu_actor() {
	lv_group_remove_all_objs(GROUP_BUTTONS);
	add_group_buttons(lv_object, ACTOR_SCREEN_BUTTON_1,
	NUMBERS_OF_BUTTONS_ACTOR);
	lv_timer_pause(lv_timer_main);
	lv_timer_resume(lv_timer_actor);
	lv_scr_load(lv_object[ACTOR_SCREEN]);
	current_menu_index = CURRENT_MENU_ACTOR_Y;
}
void draw_menu_actor_switch(bool status) {
	if (status == 1) {
		lv_obj_del_async_some(lv_object, ACTOR_SWITCH_SCREEN_BACKGROUND, 3); //BACKGROUND],BTN1,BTN2
		add_group_buttons(lv_object, ACTOR_SCREEN_BUTTON_1,
		NUMBERS_OF_BUTTONS_ACTOR);
		current_menu_index = CURRENT_MENU_ACTOR_Y;
	} else {
		para_t bg = { .width = 295, .height = 190, .align = LV_ALIGN_CENTER, .x_ofs = 0, .y_ofs = 20 };
		para_t btn_1 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = -110, .y_ofs = -10 };
		para_t btn_2 = { .width = 80, .height = 30, .align = LV_ALIGN_BOTTOM_MID, .x_ofs = 0, .y_ofs = -10 };
		lv_object[ACTOR_SWITCH_SCREEN_BACKGROUND] = create_background(lv_scr_act(), bg.width, bg.height, bg.align, bg.x_ofs, bg.y_ofs);
		lv_object[ACTOR_SWITCH_SCREEN_BUTTON_1] = create_button(lv_scr_act(), "24V", btn_1.width, btn_1.height, btn_1.align, btn_1.x_ofs,
				btn_1.y_ofs, event_handler_btn_1_actor_switch);
		lv_object[ACTOR_SWITCH_SCREEN_BUTTON_2] = create_button(lv_scr_act(), "OK", btn_2.width, btn_2.height, btn_2.align, btn_2.x_ofs,
				btn_2.y_ofs, event_handler_btn_2_actor_switch);
		lv_group_remove_all_objs(GROUP_BUTTONS);
		lv_group_add_obj(GROUP_BUTTONS, lv_object[ACTOR_SWITCH_SCREEN_BUTTON_1]);
		lv_group_add_obj(GROUP_BUTTONS, lv_object[ACTOR_SWITCH_SCREEN_BUTTON_2]);
		current_menu_index = CURRENT_MENU_ACTOR_SWITCH;
	}
}
void draw_menu_settings() {
	lv_group_remove_all_objs(GROUP_BUTTONS);
	add_group_buttons(lv_object, SETTINGS_SCREEN_BUTTON_1,
	NUMBERS_OF_OBJECTS_SETTINGS);
	lv_timer_pause(lv_timer_main);
	lv_timer_resume(lv_timer_settings_adc);
	lv_scr_load(lv_object[SETTINGS_SCREEN]);
	current_menu_index = CURRENT_MENU_MAIN_SETTINGS;
}
void draw_menu_info() {
	lv_group_remove_all_objs(GROUP_BUTTONS);
	lv_group_add_obj(GROUP_BUTTONS, lv_object[INFO_SCREEN_BUTTON_1]);
	current_menu_index = CURRENT_MENU_MAIN_INFO;
	lv_scr_load(lv_object[INFO_SCREEN]);
}
void init_lv_objects() {
	init_fonts();
	init_styles();
	init_timers();
	create_menu_main();
	create_menu_sensor();
	create_menu_actor();
	create_menu_settings();
	create_menu_info();
	draw_menu_main();
	HAL_Delay(300); /*DONT MOVE OR DELETE THIS!!!*/
}
float map_val(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void key_up() {
	cnt_standby = 0;
	switch (current_menu_index) {
	case CURRENT_MENU_MAIN:
		lv_group_focus_next(GROUP_BUTTONS);
		lv_group_focus_next(GROUP_BUTTONS);
		break;
	case CURRENT_MENU_SENSOR:
		lv_group_focus_prev(GROUP_BUTTONS);
		break;
	case CURRENT_MENU_SENSOR_MEAS_RANGE_1: //UP
		switch (current_index_measure_range) {
		case 1:
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			current_index_measure_range = 3;
			break;
		case 2:
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			current_index_measure_range = 4;
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			lv_group_focus_next(GROUP_BUTTONS);
			current_index_measure_range = 1;
			break;
		}
		break;
	case CURRENT_MENU_SENSOR_MEAS_RANGE_2:
		switch (current_index_measure_range) {
		case 1:
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			current_index_measure_range = 3;
			break;
		case 2:
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			current_index_measure_range = 4;
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			lv_group_focus_next(GROUP_BUTTONS);
			current_index_measure_range = 1;
			break;
		}
		break;
	case CURRENT_MENU_ACTOR_Y:
		lv_group_focus_prev(GROUP_BUTTONS);
		break;
	}
	delay_mili(buttons_delay_mS);
}
void key_down() {
	cnt_standby = 0;
	switch (current_menu_index) {
	case CURRENT_MENU_MAIN:
		lv_group_focus_next(GROUP_BUTTONS);
		lv_group_focus_next(GROUP_BUTTONS);
		break;
	case CURRENT_MENU_SENSOR:
		lv_group_focus_next(GROUP_BUTTONS);
		break;
	case CURRENT_MENU_SENSOR_MEAS_RANGE_1: // DWN
		switch (current_index_measure_range) {
		case 1:
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			current_index_measure_range = 5;
			break;
		case 2:
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			current_index_measure_range = 5;
			break;
		case 3:
			lv_group_focus_prev(GROUP_BUTTONS);
			lv_group_focus_prev(GROUP_BUTTONS);
			current_index_measure_range = 1;
			break;
		case 4:
			lv_group_focus_prev(GROUP_BUTTONS);
			lv_group_focus_prev(GROUP_BUTTONS);
			current_index_measure_range = 2;

			break;
		case 5:
			break;
		}
		break;
	case CURRENT_MENU_SENSOR_MEAS_RANGE_2:
		switch (current_index_measure_range) {
		case 1:
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			current_index_measure_range = 5;
			break;
		case 2:
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			lv_group_focus_next(GROUP_BUTTONS);
			current_index_measure_range = 5;
			break;
		case 3:
			lv_group_focus_prev(GROUP_BUTTONS);
			lv_group_focus_prev(GROUP_BUTTONS);
			current_index_measure_range = 1;
			break;
		case 4:
			lv_group_focus_prev(GROUP_BUTTONS);
			lv_group_focus_prev(GROUP_BUTTONS);
			current_index_measure_range = 2;

			break;
		case 5:
			break;
		}
		break;
	case CURRENT_MENU_ACTOR_Y:
		lv_group_focus_next(GROUP_BUTTONS);
		break;
	}
	delay_mili(buttons_delay_mS);
}
void key_left() {
	cnt_standby = 0;
	switch (current_menu_index) {
	case CURRENT_MENU_SENSOR_MEAS_RANGE_1:
		current_index_measure_range--;
		if (current_index_measure_range <= 0)
			current_index_measure_range = NUMBERS_OF_BUTTONS_SENSOR_SPINBOX;
		break;
	case CURRENT_MENU_SENSOR_MEAS_RANGE_2:
		current_index_measure_range--;
		if (current_index_measure_range <= 0)
			current_index_measure_range = NUMBERS_OF_BUTTONS_SENSOR_SPINBOX;
		break;
	}
	delay_mili(buttons_delay_mS);
}
void key_right() {
	cnt_standby = 0;
	switch (current_menu_index) {
	case CURRENT_MENU_SENSOR_MEAS_RANGE_1:
		current_index_measure_range++;
		if (current_index_measure_range >= NUMBERS_OF_BUTTONS_SENSOR_SPINBOX+1)
			current_index_measure_range = 1;
		break;
	case CURRENT_MENU_SENSOR_MEAS_RANGE_2:
		current_index_measure_range++;
		if (current_index_measure_range >= NUMBERS_OF_BUTTONS_SENSOR_SPINBOX+1)
			current_index_measure_range = 1;
		break;
	}
	delay_mili(buttons_delay_mS);
}
