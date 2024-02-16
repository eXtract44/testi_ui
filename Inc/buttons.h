/*
 * buttons.h
 *
 *  Created on: 22.07.2021
 *      Author: Alex
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#define BTN_PORT_LEFT GPIOC
#define BTN_PIN_LEFT 

#define BUTTON_LEFT HAL_GPIO_ReadPin(BTN_PORT_LEFT, BTN_PIN_LEFT)
#define BUTTON_UP HAL_GPIO_ReadPin(GPIOC, BTN_PIN_UP)
#define BUTTON_MID HAL_GPIO_ReadPin(GPIOC, BTN_PIN_LEFT)
#define BUTTON_RIGHT HAL_GPIO_ReadPin(GPIOC, BTN_PIN_LEFT)
#define BUTTON_DOWN HAL_GPIO_ReadPin(GPIOC, BTN_PIN_LEFT)


enum buttons{
BUTTON_LEFT,
BUTTON_UP,
BUTTON_MID,
BUTTON_RIGHT,
BUTTON_DOWN,
};



#endif /* INC_BUTTONS_H_ */
