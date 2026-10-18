#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f1xx_hal.h" 
#include "MyRTC.h"

extern UART_HandleTypeDef huart1;

void Serial_Init(void);


#endif
