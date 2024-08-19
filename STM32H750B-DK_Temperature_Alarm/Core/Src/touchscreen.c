/**
  ******************************************************************************
  * @file    BSP/Src/touchscreen.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the touchscreen driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "touchscreen.h"

/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private Structures and Enumerations ------------------------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
TS_State_t  TS_State;
uint32_t ts_status;

/* Private variables ---------------------------------------------------------*/
/* Static variable holding the current touch color index : used to change color at each touch */
TS_Init_t hTS;
uint16_t x, y;

// Coordinates of button for increasing alarm temperature
Point button_up[3] = {
			{435, 101},
			{455, 141},
			{415, 141}
	};

// Coordinates of button for decreasing alarm temperature
Point button_down[3] = {
				{435, 251},
				{455, 211},
				{415, 211}
		};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Touchscreen_template_init(void) {
	  ts_status = BSP_ERROR_NONE;
	  uint32_t x_size, y_size;

	  BSP_LCD_GetXSize(0, &x_size);
	  BSP_LCD_GetYSize(0, &y_size);

	  hTS.Width = x_size;
	  hTS.Height = y_size;
	  hTS.Orientation =TS_SWAP_XY ;
	  hTS.Accuracy = 5;

	  /* Touchscreen initialization */
	  ts_status = BSP_TS_Init(0, &hTS);

	  if(ts_status == BSP_ERROR_NONE)
	  {
		/* Clear the LCD */
		UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

		/* Set Temperature Alarm description */
		UTIL_LCD_FillRect(0, 0, x_size, 80, UTIL_LCD_COLOR_RED);
		UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
		UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_RED);
		UTIL_LCD_SetFont(&Font24);
		UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"Temperature Alarm", CENTER_MODE);
		UTIL_LCD_SetFont(&Font20);
		UTIL_LCD_DisplayStringAt(0, 131, (uint8_t *)"Current Temperature", CENTER_MODE);
		UTIL_LCD_SetFont(&Font12);
		UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"Please use the touchscreen to", CENTER_MODE);
		UTIL_LCD_DisplayStringAt(0, 45, (uint8_t *)"change at what temperature", CENTER_MODE);
		UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"the alarm will trigger.", CENTER_MODE);

		// Draw button border
		UTIL_LCD_DrawRect(400, 90, 70, y_size - 100, UTIL_LCD_COLOR_RED);
		UTIL_LCD_DrawRect(401, 91, 68, y_size - 102, UTIL_LCD_COLOR_RED);

		// Draw buttons
		UTIL_LCD_FillPolygon(button_up, 3, UTIL_LCD_COLOR_BLACK);
		UTIL_LCD_FillPolygon(button_down, 3, UTIL_LCD_COLOR_BLACK);
	  }
}

void Draw_temperature(int temp_int, int temp_frac) {
	static char buffer[8];

	sprintf(buffer, "%d.%02d", temp_int, temp_frac);

	UTIL_LCD_SetFont(&Font24);
	UTIL_LCD_FillRect(105, 151, 266, 50, UTIL_LCD_COLOR_RED);
	UTIL_LCD_DisplayStringAt(0, 166, (uint8_t *)buffer, CENTER_MODE);
}

Button_Pressed_Typedef Button_check(void) {
	ts_status = BSP_TS_GetState(0, &TS_State);

	if(TS_State.TouchDetected) {
		x = TS_State.TouchX;
		y = TS_State.TouchY;

		if (x <= 470 && x >= 400 && y <= 141 && y >= 91) {
			return BUTTON_PRESSED_UP;
		}
		if (x <= 470 && x >= 400 && y <= 261 && y >= 211) {
			return BUTTON_PRESSED_DOWN;
		}
	}

	return BUTTON_PRESSED_NONE;
}

void Draw_threshold(int16_t alarm_temp) {
	static char buffer[8];

	sprintf(buffer, "%d", alarm_temp);

	UTIL_LCD_SetFont(&Font24);
	UTIL_LCD_FillRect(405, 151, 60, 50, UTIL_LCD_COLOR_RED);
	UTIL_LCD_DisplayStringAt(195, 166, (uint8_t *)buffer, CENTER_MODE);
}

void Draw_buttons(Button_Pressed_Typedef current_state, Button_Pressed_Typedef prev_state) {
	switch(current_state) {
		case BUTTON_PRESSED_NONE:
			if (prev_state == BUTTON_PRESSED_UP) {
				UTIL_LCD_FillPolygon(button_up, 3, UTIL_LCD_COLOR_BLACK);
			} else {
				UTIL_LCD_FillPolygon(button_down, 3, UTIL_LCD_COLOR_BLACK);
			}
		break;
		case BUTTON_PRESSED_UP:
			if (prev_state == BUTTON_PRESSED_DOWN) {
				UTIL_LCD_FillPolygon(button_down, 3, UTIL_LCD_COLOR_BLACK);
			}
			UTIL_LCD_FillPolygon(button_up, 3, UTIL_LCD_COLOR_DARKRED);
		break;
		case BUTTON_PRESSED_DOWN:
			if (prev_state == BUTTON_PRESSED_UP) {
				UTIL_LCD_FillPolygon(button_up, 3, UTIL_LCD_COLOR_BLACK);
			}
			UTIL_LCD_FillPolygon(button_down, 3, UTIL_LCD_COLOR_DARKRED);
		break;
	}
}

/**
  * @}
  */

/**
  * @}
  */
