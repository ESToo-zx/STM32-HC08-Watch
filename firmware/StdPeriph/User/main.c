#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "menu.h"
#include "Timer.h"
#include "Key.h"
#include "MenuFSM.h"
#include "countDown.h"
#include "dino.h"

//void EXTI0_NVIC_Init(void);
void isEnter_StandByMode(void);

uint8_t free_sec; //停机计数

/*W25Q64模块引脚：CS->PA9; DO->PB14; DI->PB15; SLC->PB13*/
/*暂时不考虑W25Q64模块*/

/*原有LED引脚需要重新配置，原有关机/开机电路及引脚需要重新配置*/
/*LED删除,开关机接口改为PA11与PA12*/

/*测试发现PB12,13,14,15引脚是坏的。。。是说之前接灯怎么不亮*/

/*日志（第一版为git 11e649f 后分支newMain01 后首次提交的版本）
* Log01 -> 主要改动：1.更改配置KEY3为PA0，输入下拉模式，原先为输入下拉
					 2.移除W25Q64模块，不需要这个模块
					 3.电量检测ADC输入引脚配置由PA0改为PA1
					 4.开关机电路接口调整至PA11、12
					 5.长时间不操作由进入停止模式改为进入待机模式，删除key3的GPIO配置，函数暂未删除(注释掉)，只取消声明与调用

* Log02 -> 主要改动：1.新建Serial.c与.h，包含蓝牙模块初始化函数，PA9为Tx，PA10为Rx
					 2.MyRTC.c新增 MyRTC_ReadSerialTime() 函数
					 3.实测可以在串口调试助手里发送ASCII类型的Unix时间戳来更改全局时间数组
					 4.Hint：在发送数据时建议带上换行，否则中断函数可能无法判断是否接受完毕
					 5.BUG: 复位后首次发送时间戳会无响应

* Log03 -> 主要改动：1.从网上复制了一个谷歌小恐龙游戏过来，dino.c文件内为几乎未修改的游戏代码，没有任何注释
					 2.代码已验证可以正常执行
					 3.菜单界面中还有ALARM没有实现，在手表本地实现闹钟是不可能的了，既没有蜂鸣器也没有震动，板子打完了已经硬件锁死
					 4.闹钟功能通过蓝牙模块触发手机通知似乎不太可行，因为蓝牙模块通过usart串口通信，待机模式所有外设停止工作


*/

/*引脚分配
PB1-上拉输入-KEY1;  PA6-上拉输入-KEY2; PA0-下拉输入-KEY3 
*/

int main(void)
{
	/*OLED初始化*/
	OLED_Init();
	OLED_Clear();
	Peripheral_Init();
	//EXTI0_NVIC_Init();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//开启PWR时钟
	
	int DisplayPage;
	
	
	
	extern int press_time;
	extern uint8_t Key_Num;
	Timer_Init();
	while (1)
	{
		DisplayPage=Interaction_MainPage();
		if(DisplayPage==1){Enter_Menu();}//菜单
		else if(DisplayPage==2){Interaction_SettingPage();}//设置
	}
}

// TIM2配置为中断频率为1000Hz，1ms触发一次
// 定时器中断函数，可以复制到使用它的地方
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		
		Key3_Tick();// 检测KEY3是否长按
		Key_Tick();// 检测按键状态
		StopWatch_Tick();
		CountDown_Tick();
		Dino_Tick();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		isEnter_StandByMode(); //判断是否进入停止模式
	}
}

////初始化PA0引脚的中断
//void EXTI0_NVIC_Init(void)
//{
//	// 配置EXTI模块
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); //AFIO将line0 选择为GPIOA，对应PA0

//	
//	EXTI_InitTypeDef EXTI_InitStructure;
//	
//	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_Init(&EXTI_InitStructure);
//	
//	//配置中断
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级
//	
//	NVIC_InitTypeDef NVIC_InitStruct;

//	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

//	NVIC_Init(&NVIC_InitStruct);
//	
//}

void isEnter_StandByMode(void)
{
  
	static uint16_t count = 0;
  if (++count >= 1000) {                 //TIM2中断频率为1000Hz(1ms), 分频为1s
      count = 0;
		if (++free_sec >= 120) //120秒未操作待机
		{
			free_sec = 0;
			OLED_Clear();
			OLED_Update();
			PWR_WakeUpPinCmd(ENABLE);//配置PA0为WakeUp引脚
			PWR_EnterSTANDBYMode();	//STM32进入待机模式，并等待WakeUp/复位按钮唤醒
			//SystemInit();										//唤醒后，要重新配置时钟
		}
	}
}

// 中断响应函数
/*什么都不干，在停机模式下按下key3触发外部中断退出停机模式*/
//void EXTI0_IRQHandler(void)
//{
//	if(EXTI_GetFlagStatus(EXTI_Line0) == SET) { 
 //     EXTI_ClearFlag(EXTI_Line0); // 清除中断标志位
//  }
//}
