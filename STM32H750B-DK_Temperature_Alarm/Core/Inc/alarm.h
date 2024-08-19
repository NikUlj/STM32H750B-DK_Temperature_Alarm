/*
 * alarm.h
 *
 *  Created on: Aug 16, 2024
 *      Author: Nik
 */

#ifndef INC_ALARM_H_
#define INC_ALARM_H_

typedef enum
{
  ALARM_NOT_ACTIVE        = 0x00U,
  ALARM_IS_ACTIVE         = 0x01U
} Alarm_State_Typedef;

typedef struct
{
	int period;
	int pulse;
} Note;

extern Alarm_State_Typedef alarm_state;

void Alarm(int alarm_temp, float temp);
void Set_alarm(Note note);


#endif /* INC_ALARM_H_ */

