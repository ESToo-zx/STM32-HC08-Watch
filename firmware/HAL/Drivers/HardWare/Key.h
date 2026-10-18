#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx_hal.h"                  // Device header
#include "Delay.h"

//void Key_Init(void);
uint8_t Key_GetNum(void);
void Key_Tick(void);
void Key3_Tick(void);
#endif
