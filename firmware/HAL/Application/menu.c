#include "menu.h"

uint8_t KeyNum;

//void Peripheral_Init(void)
//{
//	MyRTC_Init();
//	Key_Init();
//	LED_Init();
//	Serial_Init();
//	MPU6050_Init();
//	AD_Init();
//	
//}



/*----------------------------------首页时钟-------------------------------------*/

// 绘制电池图标


uint16_t ADValue;
float VBAT;
int Battery_Capacity;

void Show_Battery(void)
{
	int sum = 0;
	for(int i=0;i<100;i++)
	{
		ADValue=AD_GetValue();
		sum+=ADValue;
		
	}
	ADValue=sum/100;
	VBAT=(float)ADValue/4095*3.3;
	Battery_Capacity=(ADValue-3276)*100/819;
	if(Battery_Capacity<0)Battery_Capacity=0;
	
	//OLED_ShowNum(64,0,ADValue,4,OLED_6X8);
	//OLED_Printf(64,8,OLED_6X8,"VBAT:%.2f",VBAT);
	OLED_ShowNum(85,4,Battery_Capacity,3,OLED_6X8);
	OLED_ShowChar(103,4,'%',OLED_6X8);
	
	if(Battery_Capacity==100)OLED_ShowImage(110,0,16,16,Battery);
	else if(Battery_Capacity>=10&&Battery_Capacity<100)
	{
		OLED_ShowImage(110,0,16,16,Battery);
		OLED_ClearArea((112+Battery_Capacity/10),5,(10-Battery_Capacity/10),6);
		OLED_ClearArea(85,4,6,8);
	}
	
	else
	{
		OLED_ShowImage(110,0,16,16,Battery);
		OLED_ClearArea(112,5,10,6);
		OLED_ClearArea(85,4,12,8);
	}
};

// 绘制主界面
void Draw_MainPage(void)
{
	Show_Battery();
	MyRTC_ReadTime(); // 加载时间到全局数组
	OLED_Printf(0,0,OLED_6X8,"%d-%d-%d",MyRTC_Time[0],MyRTC_Time[1],MyRTC_Time[2]);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",MyRTC_Time[3],MyRTC_Time[4],MyRTC_Time[5]);
	OLED_ShowString(0,48,"菜单",OLED_8X16);
	OLED_ShowString(96,48,"设置",OLED_8X16);
}

int cursorPosition_M=1;

// 返回数字给main函数的DsiaplayPage
int Interaction_MainPage(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();

		if(KeyNum==1)//上一项
		{
			cursorPosition_M--;
			if(cursorPosition_M<=0)cursorPosition_M=2; // 光标选中首尾相连
		}
		else if(KeyNum==2)//下一项
		{
			cursorPosition_M++;
			if(cursorPosition_M>=3)cursorPosition_M=1; // 光标选中首尾相连
		}
		else if(KeyNum==3)//确认
		{
			OLED_Clear();
			OLED_Update();
			return cursorPosition_M; //确认光标位置，1为菜单，2为设置
		}
		else if(KeyNum==4) // 处理关机
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
		};
		
		switch(cursorPosition_M)   // 判断选中页面
		{
			case 1:
				Draw_MainPage();
				OLED_ReverseArea(0,48,32,16);  //颜色反转“菜单”
				OLED_Update();
				break;
			
			case 2:
				Draw_MainPage();
				OLED_ReverseArea(96,48,32,16);//反转“设置”
				OLED_Update();
				break;
		}
	}
}

/*----------------------------------设置界面-------------------------------------*/

void Draw_SettingPage(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,"日期时间设置",OLED_8X16);
}

int cursorPosition_S=1;
int Interaction_SettingPage(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		uint8_t cursorPosition_S_temp=0;// 确认后光标位置
		if(KeyNum==1)//上一项
		{
			cursorPosition_S--;
			if(cursorPosition_S<=0)cursorPosition_S=2;
		}
		else if(KeyNum==2)//下一项
		{
			cursorPosition_S++;
			if(cursorPosition_S>=3)cursorPosition_S=1;
		}
		else if(KeyNum==3)//确认
		{
			OLED_Clear();
			OLED_Update();
			cursorPosition_S_temp=cursorPosition_S;
		}
		
		if(cursorPosition_S_temp==1){return 0;}//按下返回键返回值为零
		else if(cursorPosition_S_temp==2){SetTime();}// 跳转设置时间页面
		
		switch(cursorPosition_S)
		{
			case 1:
				Draw_SettingPage();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
				break;
			
			case 2:
				Draw_SettingPage();
				OLED_ReverseArea(0,16,96,16);
				OLED_Update();
				break;
		}
	}
}

/*----------------------------------Stopwatch-------------------------------------*/

static uint8_t hour, min, sec;
static uint8_t cursor_position = 1;         // cursor position
uint8_t isStarted;                         // 计时状态标志位

typedef enum {
    BTN_BACK  = 1,
    BTN_START,
    BTN_STOP,
    BTN_RESET,
} SWatchItem;

// 计时函数
void StopWatch_Tick(void)
{
    static uint16_t count = 0;
    if (++count >= 1000) {                 //TIM2中断频率为1000Hz(1ms), 分频为1s
        count = 0;
        if (isStarted == START_COUNT ) {
            if (++sec >= 60) { sec = 0;   // ++sec表示先增加sec再判断
                if (++min >= 60) { min = 0;
                    if (++hour >= 100) hour = 0;
                }
            }
        }
    }
}

// 绘制界面
static void Draw_StopWatch_UI(void)
{
    OLED_ShowImage(0, 0, 16, 16, Return);
    OLED_Printf(32, 20, OLED_8X16, "%02d:%02d:%02d", hour, min, sec);
    OLED_ShowString(8,  44, "Start", OLED_8X16);
    OLED_ShowString(48, 44, "Stop", OLED_8X16);
    OLED_ShowString(88, 44, "Reset", OLED_8X16);
}

// 绘制高亮选中项
static void Draw_Swatch_Highlight(uint8_t btn)
{
    switch (btn) {
        case BTN_BACK:  OLED_ReverseArea(0,  0, 16, 16); break;
        case BTN_START: OLED_ReverseArea(8,  44, 32, 16); break;
        case BTN_STOP:  OLED_ReverseArea(48, 44, 32, 16); break;
        case BTN_RESET: OLED_ReverseArea(88, 44, 32, 16); break;
    }
}

extern uint8_t free_sec;
// 秒表主函数
int StopWatch(void)
{
    cursor_position = BTN_BACK;//选中部分
    while (1) {
				free_sec = 0;  // 秒表界面不停机
        uint8_t key = Key_GetNum();
        static uint8_t select = 0; // 最终选中项

        if (key == 1) {                         // 上一项
            cursor_position = (cursor_position == BTN_BACK) ? BTN_RESET : cursor_position - 1;
        } else if (key == 2) {                  // 下一项
            cursor_position = (cursor_position == BTN_RESET) ? BTN_BACK : cursor_position + 1;
        } else if (key == 3) {                  // 确认
            OLED_Clear();
            //OLED_Update();
            select = cursor_position;
        }

        if (select == BTN_BACK) {select = 0; return 0;}        // return to menu

        Draw_StopWatch_UI();
        Draw_Swatch_Highlight(cursor_position);
        OLED_Update();

        if (select == BTN_START) {               // Start
            isStarted = START_COUNT;
        } else if (select == BTN_STOP) {         // Stop
            isStarted = STOP_COUNT;
        } else if (select == BTN_RESET) {        // Reset
            isStarted = STOP_COUNT;
            hour = min = sec = 0;
        }
    }
}

/*----------------------------------MPU6050-------------------------------------*/

int16_t ax,ay,az,gx,gy,gz;	//MPU6050测得的三轴加速度和角速度
float roll_g,pitch_g,yaw_g;	//陀螺仪解算的欧拉角
float roll_a,pitch_a;		//加速度计解算的欧拉角
float Roll,Pitch,Yaw;		//互补滤波后的欧拉角
float a=0.9;				//互补滤波器系数
float Delta_t=0.005;		//采样周期
double pi=3.1415927;

// MPU6050数据处理函数
void MPU6050_Calculation(void)
{
	Delay_ms(5);
	MPU6050_GetData(&ax,&ay,&az,&gx,&gy,&gz);
	
	//通过陀螺仪解算欧拉角
	roll_g=Roll+(float)gx*Delta_t;
	pitch_g=Pitch+(float)gy*Delta_t;
	yaw_g=Yaw+(float)gz*Delta_t;
	
	//通过加速度计解算欧拉角
	pitch_a=atan2((-1)*ax,az)*180/pi;
	roll_a=atan2(ay,az)*180/pi;
	
	//通过互补滤波器进行数据融合
	Roll=a*roll_g+(1-a)*roll_a;
	Pitch=a*pitch_g+(1-a)*pitch_a;
	Yaw=a*yaw_g;
	
}
/*----------------------------------水平仪-------------------------------------*/
// 显示主界面
void Show_Gradienter_UI(void)
{
	MPU6050_Calculation();
	OLED_DrawCircle(64,32,30,0);			// 绘制外圆
	OLED_DrawCircle(64-Roll,32+Pitch,4,1);	// 绘制小圆
}

int Gradienter(void)
{
	while(1)
	{
		free_sec = 0; //不停机
		KeyNum=Key_GetNum();
		if(KeyNum==3)
		{
			OLED_Clear();
			OLED_Update();
			return 0;
		}
		OLED_Clear();
		Show_Gradienter_UI();
		OLED_Update();
	}
}


