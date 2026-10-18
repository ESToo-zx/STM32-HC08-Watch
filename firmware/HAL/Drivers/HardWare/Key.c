#include "key.h"

uint8_t Key_Num;

//void Key_Init(void)
//{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//}

uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	if(Key_Num)
	{
		Temp=Key_Num;
		Key_Num=0;
		return Temp;
	}
	else
	{
		return 0;
	}
}

int press_time;
extern uint8_t free_sec;
//检测KEY3是否长按
void Key3_Tick(void)
{
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) != 0)
	{
		free_sec = 0;
		press_time++;
	}
	
	if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) != 1))
	{
		press_time=0;
	}
};
uint8_t Key_GetState(void)
{
	
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0)
	{
		free_sec = 0;
		return 1;
	}
	else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 0)
	{
		free_sec = 0;
		return 2;
	}
	
	else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) != 0)&&press_time>1000)
	{
		free_sec = 0;
		return 4;// 长按时返回状态为4
	}
	else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) != 0)
	{
		free_sec = 0;
		return 3;
	}
	
	else
	{
		return 0;
	}
	
}

void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t CurrentState,PreState;
	Count++;
	if(Count>=20)
	{
		Count=0;
		PreState=CurrentState;
		CurrentState=Key_GetState();
		if(PreState!=0&&CurrentState==0)
		{
			Key_Num=PreState;
		}
	}
}
