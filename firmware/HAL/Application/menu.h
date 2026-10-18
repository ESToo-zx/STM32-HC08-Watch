#ifndef __MENU_H
#define __MENU_H

#include "stm32f1xx_hal.h"                  // Device header
#include "OLED.h"
#include "OLED_Data.h"
#include "MyRTC.h"                                                                                                                                                                                                     
#include "Key.h"
#include "SetTime.h"
#include "menu.h"
#include <math.h>
#include "MPU6050.h"
#include "Delay.h"
#include "AD.h"
#include "dino.h"


#define START_COUNT                     1 // 开始计数
#define STOP_COUNT                      0 // 停止计数

//void Peripheral_Init(void);
void Draw_MainPage(void);
int Interaction_MainPage(void);

void Draw_SettingPage(void);
int Interaction_SettingPage(void);

void StopWatch_Tick(void);
int StopWatch(void);

int Gradienter(void);

//int Menu(void);

#endif
