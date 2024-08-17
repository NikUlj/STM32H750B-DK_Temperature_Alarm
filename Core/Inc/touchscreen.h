/*
 * touchscreen.h
 *
 *  Created on: Aug 15, 2024
 *      Author: Nik
 */

#ifndef INC_TOUCHSCREEN_H_
#define INC_TOUCHSCREEN_H_

typedef enum
{
  BUTTON_PRESSED_NONE     = 0x00U,
  BUTTON_PRESSED_UP       = 0x01U,
  BUTTON_PRESSED_DOWN     = 0x02U
} Button_Pressed_Typedef;

#define  CIRCLE_RADIUS        40
#define  LINE_LENGHT          30

#define  CIRCLE_XPOS(i)       ((i * 480) / 5)
#define  CIRCLE_YPOS(i)       (272 - CIRCLE_RADIUS - 60)

void Touchscreen_demo(void);
void Touchscreen_DrawBackground_Circles(uint8_t state);
uint8_t TouchScreen_GetTouchPosition(void);
void Touchscreen_template_init(void);
void Draw_temperature(int temp_int, int temp_frac);
void Draw_buttons(Button_Pressed_Typedef button_state, Button_Pressed_Typedef prev_state);
Button_Pressed_Typedef Button_check(void);
void Draw_threshold(int16_t alarm_temp);

#endif /* INC_TOUCHSCREEN_H_ */
