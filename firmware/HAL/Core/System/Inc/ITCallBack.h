#ifndef __ITCALLBACK_H
#define __ITCALLBACK_H

#include "stm32f1xx_hal.h"
#include "MyRTC.h"
#include "OLED.h"
#include "menu.h"
#include "Key.h"
#include "MenuFSM.h"
#include "countDown.h"
#include "dino.h"
#include "main.h"
extern UART_HandleTypeDef huart1;
extern uint8_t RxByte;        // 每次在Rx上读取到的字节



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);    //USART中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);//TIM2中断回调函数

#endif
