/*
 * menu.h
 *
 *  Created on: 05.08.2021
 *      Author: Alex
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_



void draw_button(uint16_t x, uint16_t y, uint16_t color, uint16_t bg, char c[]);
//void draw_button_helper(char c);
void draw_m_frame(uint16_t x, uint16_t y);
void draw_s_frame(uint16_t x, uint16_t y);
void draw_legende(uint16_t y, uint16_t cur_page);
void draw_menu_home();
void draw_menu_antrieb_r();
void draw_menu_antrieb_s();
void draw_menu_raumregler_aktiv();
void draw_menu_raumregler_passiv();
void draw_menu_raumtemp();
void draw_menu_kanaltemp();
void draw_menu_feuchte();
void draw_menu_druckluft();
void draw_menu_druckwasser();
void draw_menu_luftquali();
void draw_menu_helligkeit();
void draw_menu_ysensor();
void draw_premenu_ysensor();
void draw_graphic_m(uint16_t x, uint16_t y, uint16_t color);
//void draw_graphic_h(uint16_t x, uint16_t y, uint16_t color);
void draw_pt100(uint16_t x, uint16_t y, uint16_t color);
void draw_poti(uint16_t x, uint16_t y, uint16_t color);
void draw_pt100menu();
void draw_start();
void draw_infoMenu1();
void draw_infoMenu2();
void draw_menu_tools();
void draw_infoMenuUp();
void select_main_menu(uint8_t menu_page);
void select_sensor_submenu(uint8_t menu_page);
void select_antrieb_submenu(uint8_t menu_page);

void draw_antriebAnim(uint16_t x, uint16_t y, int16_t val);
void draw_start_button(uint16_t color);



#endif /* INC_MENU_H_ */
