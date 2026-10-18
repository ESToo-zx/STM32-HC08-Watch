#include "ITCallBack.h"
extern uint8_t free_sec; //停机计数
volatile uint8_t rtc_update_pending = 0;
uint32_t pending_timestamp;

void isEnter_StandByMode(void);

/*================================USART1模块读取数据回调函数=================================*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint8_t  rx_buf[16];         // 字节缓冲区（Unix时间戳最长10位 + 换行符）
	static uint8_t  rx_index = 0;       // 当前帧已缓存的字节数                                       
	uint32_t timestamp;                 // 解析后得到的 Unix 时间戳             

	// 情况A：收到帧结束标志 '\r' 或 '\n'
	if (RxByte == '\n' || RxByte == '\r'||rx_index == 10)
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
				rtc_update_pending = 1;
				pending_timestamp = timestamp;
				MyRTC_ReadSerialTime(timestamp);
				MyRTC_SetTime(&hrtc);
			}

			rx_index = 0;                            // 重置，准备接收下一帧
		}
	}
	//情况B：正常数字字节，存入缓冲区，防止非数字字节写入
	else if (RxByte >= '0' && RxByte <= '9' && rx_index < sizeof(rx_buf) - 1)
	{
		rx_buf[rx_index] = RxByte;                     // 缓存字节 
		rx_index++;
	}
    else
    {
        // 缓冲区满，丢弃整帧，重置以接收下一帧
        rx_index = 0;
    }

		__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_RXNE);
		HAL_UART_Receive_IT(&huart1, &RxByte, sizeof(RxByte));//准备Rx读取中断
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	Key3_Tick();// 检测KEY3是否长按
  Key_Tick();// 检测按键状态
	StopWatch_Tick();
	CountDown_Tick();
	Dino_Tick();
	isEnter_StandByMode(); //判断是否进入停止模式
	
}

