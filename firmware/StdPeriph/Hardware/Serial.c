#include "Serial.h"

//PA9 -> USART1_Tx;  PA10 -> USART1_Rx
/*初始化函数*/
void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// #1. 配置GPIO引脚
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// #2. 配置USART串口
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	// #3. 初始化NVIC中断
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}

/**
  * 函    数：USART1_IRQHandler
  * 功    能：USART1 接收中断服务函数
  * 帧格式：手机 -> HC-08 蓝牙 -> USART1，ASCII 时间戳以换行符结尾
  *          例："1754567890\n" -> 解析为 uint32_t = 1754567890
  * 说    明：中断每次只触发 1 字节，由 static 缓冲区拼出完整一帧后再解析
  */
void USART1_IRQHandler(void)
{
	static uint8_t  rx_buf[16];         // 字节缓冲区（Unix时间戳最长10位 + 换行符）
	static uint8_t  rx_index = 0;       // 当前帧已缓存的字节数                  
	uint8_t byte;                       // 本次收到的单字节                      
	uint32_t timestamp;                 // 解析后得到的 Unix 时间戳             
//	// 非接收中断，直接退出
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) != SET)
//	{
//		return;
//	}

	byte = USART_ReceiveData(USART1);               // 1. 读出当前字节

	// 情况A：收到帧结束标志 '\r' 或 '\n'
	if (byte == '\n' || byte == '\r'||rx_index == 10)
	{
		if (rx_index > 0)                            // 缓冲区不为空才解析
		{
			rx_buf[rx_index] = '\0';                 // 字符串终止符 

			// 将 ASCII 数字串转换为 uint32_t
			timestamp = 0;
			for (uint8_t i = 0; i < rx_index; i++)
			{
				if (rx_buf[i] >= '0' && rx_buf[i] <= '9')
				{
					timestamp = timestamp * 10 + (rx_buf[i] - '0');
				}
			}

			/* 解析成功则更新 RTC */
			if (timestamp > 0)
			{
				MyRTC_ReadSerialTime(timestamp);
				MyRTC_SetTime();
			}

			rx_index = 0;                            // 重置，准备接收下一帧
		}
	}
	//情况B：正常数字字节，存入缓冲区，防止非数字字节写入
	else if (byte >= '0' && byte <= '9' && rx_index < sizeof(rx_buf) - 1)
	{
		rx_buf[rx_index] = byte;                     // 缓存字节 
		rx_index++;
	}
    else
    {
        // 缓冲区满，丢弃整帧，重置以接收下一帧
        rx_index = 0;
    }

	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}
