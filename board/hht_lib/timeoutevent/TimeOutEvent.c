/**
  ******************************************************************************
  * @file    TimeOutEvent.c
  * @author  HuyHT1-VF Team
  * @version V1.1.0
  * @date    11-march-2019
  * @brief   device mounted on STM32H743I-NEO boards.
  */

#include <stdint.h>
#include "TimeOutEvent.h"
#include "sys_tick.h"

void ToEUpdate(TIMEOUT_TypeDef *Time, uint32_t To_Value){
	Time->TSta = millis();
	Time->To_Value = To_Value;
	Time->State = TIMEOUT_STATE_RUNNING;
}

TimeOutState_TypeDef ToEExpired(TIMEOUT_TypeDef *Time){
	if((Time->State != TIMEOUT_STATE_END) && ((millis() - Time->TSta) >= Time->To_Value)){
		Time->State = TIMEOUT_STATE_END;
	}
	return Time->State;
}

void ToEDisable(TIMEOUT_TypeDef *Time){
	Time->State = TIMEOUT_STATE_END;
}

TimeOutState_TypeDef ToEGetStatus(TIMEOUT_TypeDef *Time){
	return Time->State;
}

uint32_t ToERemain(TIMEOUT_TypeDef *Time) {
	if(Time->State == TIMEOUT_STATE_END) return 0;
	if((millis() - Time->TSta) >= Time->To_Value) return 0;
	return (Time->TSta + Time->To_Value - millis());
}

void ToERefresh(TIMEOUT_TypeDef *Time){
	Time->TSta = millis();
}
