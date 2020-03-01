/**
  ******************************************************************************
  * @file    TimeOutEvent.h
  * @author  HuyHT1-VF Team
  * @version V1.1.0
  * @date    1-March-2019
  * @brief   This file contains the common defines and functions prototypes for
  *          the TimeOutEvent.c driver.
  ******************************************************************************
  */

#ifndef _TIMEOUT_EVENT_H_
#define _TIMEOUT_EVENT_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include <stdint.h>
   
#define TIMEOUT_DEFAULT     {\
                              .TSta = 0,\
                              .To_Value = 0,\
                              .State = TIMEOUT_STATE_RUNNING\
                            }   

typedef enum
{
	TIMEOUT_STATE_END = 0,
	TIMEOUT_STATE_RUNNING
}TimeOutState_TypeDef;

typedef struct{
	uint32_t 	TSta;   //Time Start
	uint32_t 	To_Value;   //Timeout_Value
	TimeOutState_TypeDef 	State;
}TIMEOUT_TypeDef;

TimeOutState_TypeDef ToEExpired(TIMEOUT_TypeDef *Time);
void ToEUpdate(TIMEOUT_TypeDef *Time, uint32_t Tio);
void ToEDisable(TIMEOUT_TypeDef *Time);
TimeOutState_TypeDef ToEGetStatus(TIMEOUT_TypeDef *Time);
uint32_t ToERemain(TIMEOUT_TypeDef *Time);
void ToERefresh(TIMEOUT_TypeDef *Time);

#ifdef __cplusplus
}
#endif

#endif	/* _TIMEOUT_EVENT_H_ */
