/*
 * alarm.c
 *
 *  Created on: Aug 16, 2024
 *      Author: Nik
 */
#include "alarm.h"
#include "tim.h"

Alarm_State_Typedef alarm_state = ALARM_NOT_ACTIVE;

// Two notes a half step apart
const Note note1 = {477, 238};
const Note note2 = {506, 253};

void Alarm(int alarm_temp, float temp) {
	static uint8_t current_note = 0;

	switch(alarm_state){
	// If alarm is not active and temperature is above alarm temperature, active it
		case ALARM_NOT_ACTIVE:
			if (temp >= (float)alarm_temp) {
				HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_SET);
				Set_alarm(note1);
				alarm_state = ALARM_IS_ACTIVE;
			}
		break;
		case ALARM_IS_ACTIVE:
			// If alarm is active and temperature is below alarm temperature, deactivate it
			if (temp < (float)alarm_temp) {
				current_note = 0;
				HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
				alarm_state = ALARM_NOT_ACTIVE;
			// If alarm is active and temperature is still above alarm temperature, change note
			} else {
				if (current_note == 0) {
					Set_alarm(note2);
				} else {
					Set_alarm(note1);
				}
				current_note = 1 - current_note;
			}
		break;
	}
}

void Set_alarm(Note note) {
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4); // stop generation of pwm
	TIM_OC_InitTypeDef sConfigOC;
	htim2.Init.Period = note.period; // set the period duration
	HAL_TIM_PWM_Init(&htim2); // reinititialise with new period value
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = note.pulse; // set the pulse duration
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4); // start pwm generation
}
