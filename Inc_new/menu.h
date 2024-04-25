#ifndef INC_MENU_H_
#define INC_MENU_H_


//COORDINATES X, Y//

#define LEFT_CORNER_X 0
#define TOP_CORNER_Y 0

#define BTN_LEFT_BOTTOM_X 30
#define BTN_LEFT_BOTTOM_Y 30

#define BTN_MID_BOTTOM_X 30
#define BTN_MID_BOTTOM_Y 30

#define BTN_RIGHT_BOTTOM_X 30
#define BTN_RIGHT_BOTTOM_Y 30


//SIZES IN PX//
#define DISPLAY_WIDTH 320  //px
#define DISPLAY_HEIGHT 240  //px

#define MAX_FRAME_SIZE_BTN 10  //px
#define MAX_WIDTH_SIZE_BTN DISPLAY_WIDTH / 2  //px
#define MAX_HEIGHT_SIZE_BTN DISPLAY_HEIGHT / 2  //px

#define BTN_WIDTH_LARGE 80
#define BTN_HEIGHT_LARGE 35
#define BTN_WIDTH_MEDIUM 50
#define BTN_HEIGHT_MEDIUM 30
#define BTN_WIDTH_SMALL 40
#define BTN_HEIGHT_SMALL 25

#define FRAME_WIDTH_MEDIUM 90
#define FRAME_HEIGHT_MEDIUM 30


//#define COLOR_BLUE_MENU		  0x191970
#define COLOR_MENU_BG         0xc4
#define COLOR_MENU_BLOCK      0x05FE
#define COLOR_MENU_BAR   0xe9
#define COLOR_MENU_BUTTON   0x5D28



enum currentMenu{
MAIN_MENU,
SENSOR_MENU,
ACTOR_MENU,
INFO_MENU,
SETTING_MENU
};

#endif /* INC_MENU_H_ */
