#include "countDown.h"

int8_t SettingTime[] = {0,0,0};         //设定的时分秒
int8_t ShowTime[] = {0,0,0};            //显示的时分秒

static uint8_t cursor_position = 1;     // cursor position
static uint8_t select = 0;              // 最终选中项

static uint8_t isStarted;               // 计时状态标志位

static uint8_t currentState;            //状态标志位
static uint8_t currentPage;             //页面标志位

extern uint8_t free_sec;                //停机自增计数


// 倒计时函数
void CountDown_Tick(void)
{
    if (currentState == CD_RUNNING){
        static uint16_t count = 0;
        if (++count >= 1000) {                 //TIM2中断频率为1000Hz(1ms), 分频为1s
            count = 0;
            if (isStarted == START_COUNT&& currentPage == MainPage ) 
            {
                ShowTime[2]--;
                if (ShowTime[2] < 0) 
                { 
                    ShowTime[2] = 59; 
                    ShowTime[1]--;
                    if (ShowTime[1] < 0) 
                    { 
                        ShowTime[1] = 59;
                        ShowTime[0]--;
                        if (ShowTime[0] < 0) 
                        {
                            ShowTime[0] = 99;
                        }
                    }
                }
            }
        } 
    }
}

// 绘制界面
/*交互逻辑：在倒计时主界面光标移动到setting时进入page2
            1. 设置状态，光标跳转到秒处，key1加1秒，key2减一秒，小于零跳转59秒，按key3确认，转到设置分钟
            2. until设置完小时后，退出设置模式，返回主界面
*/
static void Draw_CountDown_UI(void)
{
    if (currentPage == MainPage){

        OLED_ShowImage(0, 0, 16, 16, Return);
        OLED_Printf(32, 20, OLED_8X16, "%02d:%02d:%02d", ShowTime[0], ShowTime[1], ShowTime[2]);
        OLED_ShowImage(8, 44, 16, 16, Player_Start);
        OLED_ShowImage(40, 44, 16, 16, Player_Stop);
        OLED_ShowImage(72, 44, 16, 16, Player_Restart);
        OLED_ShowImage(104, 44, 16, 16, Setting);

    } else if (currentPage == SettingPage){

        OLED_Clear();   //防止mainpage图像残留
        OLED_Printf(32, 20, OLED_8X16, "%02d:%02d:%02d", SettingTime[0], SettingTime[1], SettingTime[2]);
    }
}
    

// 绘制高亮选中项
static void Draw_CountDown_Highlight(uint8_t btn)
{
    if(currentPage == MainPage){

        switch (btn) {
            case BTN_BACK:  OLED_ReverseArea(0,  0, 16, 16); break;
            case BTN_START: OLED_ReverseArea(8,  44, 16, 16); break;
            case BTN_STOP:  OLED_ReverseArea(40, 44, 16, 16); break;
            case BTN_RESET: OLED_ReverseArea(72, 44, 16, 16); break;
            case BTN_SETTING: OLED_ReverseArea(104, 44, 16, 16); break;
        }

    } else if (currentPage == SettingPage ){

        switch (btn) {
            case NUM_HOUR:  OLED_ReverseArea(32,  20, 16, 16);OLED_Update(); break;
            case NUM_MIN: OLED_ReverseArea(56, 20, 16, 16);OLED_Update(); break;
            case NUM_SEC: OLED_ReverseArea(80, 20, 16, 16);OLED_Update(); break;
        }
    }
  
}

/*page1: back-start-stop-reset-setting
  page2: hour-min-sec
*/
void handleKey(void) {
    uint8_t key = Key_GetNum();
    

    switch(currentPage) {

        case MainPage:
            if (key == 1) {                         // 上一项
                cursor_position = (cursor_position == BTN_BACK) ? BTN_SETTING : cursor_position - 1;
            } else if (key == 2) {                  // 下一项
                cursor_position = (cursor_position == BTN_SETTING) ? BTN_BACK : cursor_position + 1;
            } else if (key == 3) {                  // 确认
                OLED_Clear();
                select = cursor_position;
                if (select == BTN_SETTING) {
                    cursor_position = NUM_SEC; // 光标跳转到秒处
                }
            }
            break;

        case SettingPage:

            switch (cursor_position) {
                case NUM_SEC:

                    if (key == 1) {                         // 上一项
                        SettingTime[2] = (SettingTime[2] - 1 < 0) ? 59 : SettingTime[2] - 1;
                    } else if (key == 2) {                  // 下一项
                        SettingTime[2] = (SettingTime[2] + 1 > 59) ? 0 : SettingTime[2] + 1;
                    } else if (key == 3) {                  // 确认
                        OLED_Clear();
						cursor_position = NUM_MIN; // 光标跳转到分钟处
                        //select = cursor_position;
                    }
                    break;

                case NUM_MIN:

                    if (key == 1) {                         // 上一项
                        SettingTime[1] = (SettingTime[1] - 1 < 0) ? 59 : SettingTime[1] - 1;
                    } else if (key == 2) {                  // 下一项
                        SettingTime[1] = (SettingTime[1] + 1 > 59) ? 0 : SettingTime[1] + 1;
                    } else if (key == 3) {                  // 确认
                        OLED_Clear();
						cursor_position = NUM_HOUR; // 光标跳转到小时处
                        //select = cursor_position;
                    }
                    break;
                case NUM_HOUR:

                    if (key == 1) {                         // 上一项
                        SettingTime[0] = (SettingTime[0] - 1 < 0) ? 99 : SettingTime[0] - 1;
                    } else if (key == 2) {                  // 下一项
                        SettingTime[0] = (SettingTime[0] + 1 > 99) ? 0 : SettingTime[0] + 1;
                    } else if (key == 3) {                  // 确认
                        OLED_Clear();
                        currentState = CD_IDLE;         // 退出设置状态
                        currentPage = MainPage;         // 返回主界面
                        cursor_position = BTN_START;    // 光标跳转到开始处
                        select = 0;
                        //select = cursor_position;
                        ShowTime[0] = SettingTime[0];
                        ShowTime[1] = SettingTime[1];   
                        ShowTime[2] = SettingTime[2];   //将显示时间更新至设置时间
                        
                    }
                    break;
            }
            break;
    }   
}



// 倒计时主函数
int CountDown(void)
{
    cursor_position = BTN_BACK;//选中返回
    currentPage = MainPage;
    currentState = CD_IDLE;
    while (1) {
		free_sec = 0;  // 倒计时界面不停机

        handleKey();

        if (select == BTN_BACK) {   // return to menu
            select = 0;             // 清除选中标志位
            return 0;
        }        

        Draw_CountDown_UI();
        Draw_CountDown_Highlight(cursor_position);
        OLED_Update();

        switch(select) {
            case BTN_START:
                isStarted = START_COUNT;
                currentState = CD_RUNNING;
                break;
            case BTN_STOP:
                isStarted = STOP_COUNT;
                currentState = CD_IDLE;
                break;
            case BTN_RESET:
                isStarted = STOP_COUNT;
                ShowTime[0] = SettingTime[0];
                ShowTime[1] = SettingTime[1];   
                ShowTime[2] = SettingTime[2];   //将显示时间更新至设置时间
                currentState = CD_IDLE;
                break;
            case BTN_SETTING:
                currentPage = SettingPage;
                currentState = CD_SETTING;
                break;
        }
    }
}
