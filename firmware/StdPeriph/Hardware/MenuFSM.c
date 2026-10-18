/**
 * ============================================================================
 * 滑动菜单界面 - MenuFSM.c
 * ============================================================================
 * OLED: 128x64 像素
 * 菜单项：Back、秒表、倒计时、闹钟、水平仪、游戏（共6项）
 * 按键1: 向上 | 按键2: 向下 | 按键3: 确认
 */

#include "stm32f10x.h"
#include "OLED.h"
#include "Key.h"
#include "menu.h"
#include "countdown.h"
#include "dino.h"

/*----------------------------------配置参数-------------------------------------*/

#define ITEM_COUNT   6       // 菜单项总数
#define PAGE_SIZE    4       // 每页显示项数
#define ROW_HEIGHT   16      // 每行高度 = 64/4
#define BAR_WIDTH    8        // 右侧进度条宽度
#define OLED_W       128     // OLED宽度
#define OLED_H       64      // OLED高度

extern int DisplayPage;

/* 菜单项 */
typedef enum {
    ITEM_BACK = 1,
    ITEM_STOPWATCH,
    ITEM_COUNTDOWN,
    ITEM_ALARM,
    ITEM_LEVEL,
    ITEM_GAME,
} MenuItem;

/* 状态机状态 */
typedef enum {
    STATE_IDLE,
    STATE_ANIMATING,
    STATE_CONFIRM,
} State;

/*----------------------------------菜单状态（全局）-------------------------------------*/

static State cur_state = STATE_IDLE;
static uint8_t cursor = 1;
static uint8_t page = 1;

/* 菜单项名称 */
static const char* ItemName[] = {
    "Back", "Stopwatch", "Countdown", "Alarm", "Level", "Game"
};

/*----------------------------------显示函数-------------------------------------*/

/**
 * @brief  绘制进度条
 */
static void Draw_ProgressBar(void)
{
    uint8_t bar_x = OLED_W - BAR_WIDTH;
    uint8_t bar_h = OLED_H / ITEM_COUNT;
    uint8_t bar_y = (cursor - 1) * bar_h;

    OLED_DrawRectangle(bar_x, bar_y, BAR_WIDTH, bar_h, OLED_FILLED);
    //OLED_Update();
}

/**
 * @brief  绘制菜单列表
 */
static void Draw_List(void)
{
    uint8_t start = (page == 1) ? 1 : 5; // 根据页码确定起始菜单项
    uint8_t i;

    for (i = 0; i < PAGE_SIZE; i++) {
        uint8_t item = start + i;  // 当前菜单项
        uint8_t y = i * ROW_HEIGHT;// 当前行的Y坐标

        if (item > ITEM_COUNT) break; //防止越界

        OLED_ShowString(0, y, (char*)ItemName[item - 1], OLED_8X16);  //先绘制文字再反色
        //OLED_Update();

        if (item == cursor) {
            OLED_ReverseArea(0, y, OLED_W - BAR_WIDTH- 8, ROW_HEIGHT);
            //OLED_Update();
        }
    }
}

/**
 * @brief  绘制整个菜单界面
 */
static void Draw_Menu(void)
{
    OLED_Clear();
    Draw_List();
    Draw_ProgressBar();
    OLED_Update();
}

/*----------------------------------状态处理-------------------------------------*/

static void State_Idle(void)
{
    uint8_t key = Key_GetNum();
    if (key == 0) return;

    if (key == 1) {                 // 向上
        cursor--;
        if (cursor < 1) {cursor = ITEM_COUNT; page = 2; cur_state = STATE_ANIMATING;}//跳转到最后一项
        if (cursor == 4 && page == 2) { page = 1; cur_state = STATE_ANIMATING; }//翻页

    }
    else if (key == 2) {            // 向下
        cursor++;
        if (cursor > ITEM_COUNT) {cursor = 1; page = 1; cur_state = STATE_ANIMATING;}
        if (cursor == 5 && page == 1) { page = 2; cur_state = STATE_ANIMATING; }
    }
    else if (key == 3) {            // 确认
        cur_state = STATE_CONFIRM;
    }
}

static void State_Animating(void)
{
    cur_state = STATE_IDLE;// 暂无动画效果，直接返回空闲状态
}

static void State_Confirm(void)
{
    OLED_Clear();
    OLED_Update();

    switch (cursor) { 
        case ITEM_BACK:      /*此处存放跳转页面逻辑*/   						break;  // 返回首页时钟
        case ITEM_STOPWATCH:    StopWatch();            						break;  // 秒表
        case ITEM_COUNTDOWN:    CountDown();            						break;  // 倒计时
        case ITEM_ALARM:                                						break;  // 闹钟
        case ITEM_LEVEL:        Gradienter();          					 		break;  // 水平仪
        case ITEM_GAME:  DinoGame_Pos_Init();DinoGame_Animation();  break;  // 游戏
    }

    cur_state = STATE_IDLE;
}

/*----------------------------------状态机调度-------------------------------------*/

static void (*StateHandler[])(void) = {
    [STATE_IDLE]      = State_Idle,
    [STATE_ANIMATING] = State_Animating,
    [STATE_CONFIRM]   = State_Confirm,
};

/*----------------------------------外部接口-------------------------------------*/

void Enter_Menu(void)
{
    cursor = ITEM_BACK;     // 默认选中返回
    page = 1;               // 默认显示第一页
    cur_state = STATE_IDLE; //默认状态为空闲

    while (cur_state != STATE_CONFIRM || cursor != ITEM_BACK) {

        if (cur_state < sizeof(StateHandler)/sizeof(StateHandler[0])) {

            StateHandler[cur_state]();// 调用当前状态处理函数
        }
        if (cur_state != STATE_CONFIRM) {
            Draw_Menu();
        }
    }

    //处理返回首页时钟逻辑
    if (cur_state == STATE_CONFIRM && cursor == ITEM_BACK){
        OLED_Clear();
      //  DisplayPage = Interaction_MainPage();
        //Draw_MainPage();
        OLED_Update();
    }
}
