#ifndef __AD_H
#define __AD_H

#include "stm32f1xx_hal.h"

extern ADC_HandleTypeDef hadc1;

void AD_Init(void);
uint16_t AD_GetValue(void);

#endif
