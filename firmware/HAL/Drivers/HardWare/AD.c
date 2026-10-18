#include "AD.h"           

/**
  * 函    数：AD初始化
  * 参    数：无
  * 返 回 值：无
  */
//void AD_Init(void)
//{
//	/*开启时钟*/
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//开启ADC1的时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
//	
//	/*设置ADC时钟*/
//	RCC_ADCCLKConfig(RCC_PCLK2_Div6);						//选择时钟6分频，ADCCLK = 72MHz / 6 = 12MHz
//	
//	/*GPIO初始化*/
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA1引脚初始化为模拟输入
//	
//	/*规则组通道配置*/
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);		//规则组序列1的位置，配置为通道1
//	
//	/*ADC初始化*/
//	ADC_InitTypeDef ADC_InitStructure;						//定义结构体变量
//	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//模式，选择独立模式，即单独使用ADC1
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//数据对齐，选择右对齐
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//外部触发，使用软件触发，不需要外部触发
//	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//连续转换，失能，每转换一次规则组序列后停止
//	ADC_InitStructure.ADC_ScanConvMode = DISABLE;			//扫描模式，失能，只转换规则组的序列1这一个位置
//	ADC_InitStructure.ADC_NbrOfChannel = 1;					//通道数，为1，仅在扫描模式下，才需要指定大于1的数，在非扫描模式下，只能是1
//	ADC_Init(ADC1, &ADC_InitStructure);						//将结构体变量交给ADC_Init，配置ADC1
//	
//	/*ADC使能*/
//	ADC_Cmd(ADC1, ENABLE);									//使能ADC1，ADC开始运行
//	
//	/*ADC校准*/
//	ADC_ResetCalibration(ADC1);								//固定流程，内部有电路会自动执行校准
//	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
//	ADC_StartCalibration(ADC1);
//	while (ADC_GetCalibrationStatus(ADC1) == SET);
//}

/**
  * 函    数：获取AD转换的值
  * 参    数：无
  * 返 回 值：AD转换的值，范围：0~4095
	* 使用HAL库中轮询版启动ADC函数，可更改为中断版
  */
uint16_t AD_GetValue(void)
{
	
	HAL_ADC_Start(&hadc1); // 软件触发
  HAL_ADC_PollForConversion(&hadc1, 10); // 最多等 10 ms
  return HAL_ADC_GetValue(&hadc1);             // 返回 12 位结果
	
}
