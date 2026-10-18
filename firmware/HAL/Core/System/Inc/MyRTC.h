#ifndef __MYRTC_H
#define __MYRTC_H

#include "stm32f1xx_hal.h"                  // Device header
#include <time.h>

extern uint16_t MyRTC_Time[];
extern RTC_HandleTypeDef hrtc;

void MyRTC_Init(void);
void MyRTC_SetTime(RTC_HandleTypeDef *hrtc);
void MyRTC_ReadTime(void);
void MyRTC_ReadSerialTime(uint32_t SerialUnixTime);
static HAL_StatusTypeDef My_RTC_WriteTimeCounter(RTC_HandleTypeDef *hrtc, uint32_t unix_timestamp);
static uint32_t My_RTC_ReadTimeCounter(RTC_HandleTypeDef *hrtc);

#endif
