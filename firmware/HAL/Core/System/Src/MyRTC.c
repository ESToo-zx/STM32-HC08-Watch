#include "MyRTC.h"

uint16_t MyRTC_Time[] = {2026, 8, 3, 13, 43, 13};	//定义全局的时间数组，数组内容分别为年、月、日、时、分、秒
RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};

void MyRTC_SetTime(RTC_HandleTypeDef *hrtc);				//函数声明

/**
  * 函    数：RTC设置时间
  * 参    数：RTC_HandleTypeDef *hrtc -> 指向RTC初始化结构体的指针
  * 返 回 值：无
  * 说    明：调用此函数后，全局数组里时间值将刷新到RTC硬件电路
  */
void MyRTC_SetTime(RTC_HandleTypeDef *hrtc)
{
	time_t time_cnt;		//定义秒计数器数据类型
	struct tm time_date;	//定义日期时间数据类型
	
	time_date.tm_year = MyRTC_Time[0] - 1900;		//将数组的时间赋值给日期时间结构体
	time_date.tm_mon = MyRTC_Time[1] - 1;
	time_date.tm_mday = MyRTC_Time[2];
	time_date.tm_hour = MyRTC_Time[3];
	time_date.tm_min = MyRTC_Time[4];
	time_date.tm_sec = MyRTC_Time[5];
	
	time_cnt = mktime(&time_date) - 8 * 60 * 60;	//调用mktime函数，将日期时间转换为秒计数器格式
													//- 8 * 60 * 60为东八区的时区调整
	
	My_RTC_WriteTimeCounter(hrtc, time_cnt);						//将秒计数器写入到RTC的CNT中
	
	//等待上一次操作完成
	while((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET);
}

/**
  * 函    数：RTC读取时间
  * 参    数：无
  * 返 回 值：无
  * 说    明：调用此函数后，RTC硬件电路里时间值将刷新到全局数组
  */
void MyRTC_ReadTime(void)
{
	time_t time_cnt;															//定义秒计数器数据类型
	struct tm time_date;													//定义日期时间数据类型
	
	time_cnt = My_RTC_ReadTimeCounter(&hrtc) + 8 * 60 * 60;		//读取RTC的CNT，获取当前的秒计数器（Unix时间戳（uint32_t）加8h）
																								//+ 8 * 60 * 60为东八区的时区调整
	
	time_date = *localtime(&time_cnt);						//使用localtime函数，将秒计数器转换为日期时间格式
	
	MyRTC_Time[0] = time_date.tm_year + 1900;			//将日期时间结构体赋值给数组的时间
	MyRTC_Time[1] = time_date.tm_mon + 1;
	MyRTC_Time[2] = time_date.tm_mday;
	MyRTC_Time[3] = time_date.tm_hour;
	MyRTC_Time[4] = time_date.tm_min;
	MyRTC_Time[5] = time_date.tm_sec;
}

/**
  * 函    数：RTC读取蓝牙时间
  * 参    数：uint32_t SerialUnixTime -> Unix时间戳
  * 返 回 值：无
  * 说    明：调用此函数后，蓝牙发送过来的时间戳对应时间值将刷新到全局数组
  */
void MyRTC_ReadSerialTime(uint32_t SerialUnixTime)
{
	time_t time_cnt;															//定义秒计数器数据类型
	struct tm time_date;													//定义日期时间数据类型
	
	time_cnt = SerialUnixTime + 8 * 60 * 60;		//读取RTC的CNT，获取当前的秒计数器（Unix时间戳（uint32_t）加8h）
																								//+ 8 * 60 * 60为东八区的时区调整
	
	time_date = *localtime(&time_cnt);						//使用localtime函数，将秒计数器转换为日期时间格式
	
	MyRTC_Time[0] = time_date.tm_year + 1900;			//将日期时间结构体赋值给数组的时间
	MyRTC_Time[1] = time_date.tm_mon + 1;
	MyRTC_Time[2] = time_date.tm_mday;
	MyRTC_Time[3] = time_date.tm_hour;
	MyRTC_Time[4] = time_date.tm_min;
	MyRTC_Time[5] = time_date.tm_sec;
}

//void My_HAL_RTC_SetTime(void)
//{
//	 //防止Year数组越界，只支持设置2000-2099年
//	 sDate.Year   = ((MyRTC_Time[0] - 2000) >= 0) ? MyRTC_Time[0] - 2000 : 0;   // 数组里是 2026 → HAL 存 26
//   sDate.Month  = MyRTC_Time[1];
//   sDate.Date   = MyRTC_Time[2];
//   sTime.Hours  = MyRTC_Time[3];
//   sTime.Minutes= MyRTC_Time[4];
//   sTime.Seconds= MyRTC_Time[5];
//	
//	 HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
//   HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
//}

///**
//  * 函    数：RTC读取时间
//  * 参    数：无
//  * 返 回 值：无
//  * 说    明：调用此函数后，RTC硬件电路里时间值将刷新到全局数组
//  */
//void My_HAL_RTC_ReadTime(void)
//{

//	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
//	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);// another format RTC_FORMAT_BCD

//	MyRTC_Time[0] = sDate.Year + 2000;   // HAL 存 26 → 还原成 2026
//	MyRTC_Time[1] = sDate.Month;
//	MyRTC_Time[2] = sDate.Date;
//	MyRTC_Time[3] = sTime.Hours;
//	MyRTC_Time[4] = sTime.Minutes;
//	MyRTC_Time[5] = sTime.Seconds;
//}

///**
//  * 函    数：RTC读取蓝牙传入时间戳
//  * 参    数：uint32_t SerialUnixTime: 传入uint32_t类型变量
//  * 返 回 值：无
//  * 说    明：调用此函数后，蓝牙模块接收到的时间戳经转换后将刷新到全局数组
//	*           理想状况下，要求用户传过来的时间戳为适配当地时区后的时间戳，本地不做时区转换
//  */
//void My_HAL_RTC_ReadSerialTime(uint32_t SerialUnixTime)
//{
//	struct tm time_date;													//定义日期时间数据类型
//	
//	time_date = *localtime(&SerialUnixTime);						//使用localtime函数，将秒计数器转换为日期时间格式
//	
//	MyRTC_Time[0] = time_date.tm_year + 1900;			//将日期时间结构体赋值给数组的时间
//	MyRTC_Time[1] = time_date.tm_mon + 1;
//	MyRTC_Time[2] = time_date.tm_mday;
//	MyRTC_Time[3] = time_date.tm_hour;
//	MyRTC_Time[4] = time_date.tm_min;
//	MyRTC_Time[5] = time_date.tm_sec;
//}


/**
  * 函    数：CNT写入时间戳
  * 参    数：RTC_HandleTypeDef *hrtc -> RTC初始化结构体指针；uint32_t unix_timestamp -> 时间戳
  * 返 回 值：HAL_StatusTypeDef状态
  * 说    明：HAL库 RTC_WriteTimeCounter 改写而来，用于直接写入CNT计数器
  */
static HAL_StatusTypeDef My_RTC_WriteTimeCounter(RTC_HandleTypeDef *hrtc, uint32_t unix_timestamp)
{
	uint32_t tickstart = 0U;
	//等待上一次写入完成
	tickstart = HAL_GetTick();
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }
	
  //关闭写保护
	SET_BIT(hrtc->Instance->CRL, RTC_CRL_CNF);

	//写入高16位和低16位
	hrtc->Instance->CNTH = (unix_timestamp >> 16U);
	hrtc->Instance->CNTL = (unix_timestamp & 0xFFFF);

	// 退出初始化模式（开启写保护）
	CLEAR_BIT(hrtc->Instance->CRL, RTC_CRL_CNF);

	// 等待写入完成
	tickstart = HAL_GetTick();
	while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == RESET)
	{
			if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
			{
				return HAL_TIMEOUT;
			}	
	}

	return HAL_OK;
}

/**
  * 函    数：CNT读取时间戳
  * 参    数：RTC_HandleTypeDef *hrtc -> RTC初始化结构体指针
  * 返 回 值：uint32_t timecounter -> 时间戳
  * 说    明：HAL库 RTC_ReadTimeCounter 改写而来，用于读取CNT计数器
  */
static uint32_t My_RTC_ReadTimeCounter(RTC_HandleTypeDef *hrtc)
{
  uint16_t high1 = 0U, high2 = 0U, low = 0U;
  uint32_t timecounter = 0U;

  high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
  low   = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
  high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

  if (high1 != high2)
  {
    //若发生高16位溢出，则采用high2+low的组合
    timecounter = (((uint32_t) high2 << 16U) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  {
    timecounter = (((uint32_t) high1 << 16U) | low);
  }

  return timecounter;
}


