#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include "stm32f1xx_hal.h"
#include "OLED.h"
#include "OLED_Data.h"
#include "Key.h"
#include "Delay.h"

#define START_COUNT                     1 // 开始计数
#define STOP_COUNT                      0 // 停止计数

#define MainPage                        1 // 主界面
#define SettingPage                     2 // 设置界面

typedef enum {
    BTN_BACK    = 1,            //1
    BTN_START   = 2,            //1
    BTN_STOP    = 3,            //1
    BTN_RESET   = 4,            //1
    BTN_SETTING = 5,            //1

    NUM_SEC     = 6,            //2
    NUM_MIN     = 7,            //2
    NUM_HOUR    = 8,            //2
} CDItem;

typedef enum {
    CD_IDLE = 1,                // 空闲状态
    CD_SETTING,                 // 设置状态
    CD_RUNNING,                 // 运行状态
} CDStatus;


int CountDown(void);
void CountDown_Tick(void);

#endif
