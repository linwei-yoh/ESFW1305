#include "iostm8s103f3.h"
#include "string.h"
#include "ADC.h"



static ADC_STRUCT ADC1 = ADC_STRUCT_DEFAULT;
/****************************************************************************
** 函数名称 ：static void ADC_IOConfig (void)
** 函数功能 ：ADC的IO口初始化
****************************************************************************/
static void ADC_IOConfig (void)
{	
	PD_DDR_DDR3 = 0;				//配置PD3为ADIN4
	PD_CR1_C13  = 0;				//浮空输入
	PD_CR2_C23  = 0;				//禁止外部中断	
	ADC_TDRL   |= 1 << 4;			//禁止施密特触发器，降低功耗	
}

/****************************************************************************
** 函数名称 ：static void ADC_RegConfig (void)
** 函数功能 ：ADC寄存器初始化
****************************************************************************/
static void ADC_RegConfig (void)
{	
	CLK_PCKENR2 |= 1 << 3;			//开启ADC时钟						
	
	ADC_CR1_ADON = 0;				//关闭ADC
	
	ADC_CSR = (0 << 7)|				//清除EOC标志
			  (1 << 5)|				//使能EOC事件中断
			  (0 << 4);				//禁止ADC看门狗中断
	
	ADC_CR1 = (2 << 4)|				//Fadc = Fmaster/4
			  (0 << 1);				//单次转换模式
	
	ADC_CR2 = (0 << 6)|				//禁止外部触发转换
			  (1 << 3)|				//数据右对齐
			  (0 << 1);				//禁止扫描模式
	
	ADC_CR3 = 0 << 7;				//数据缓存禁止
	
	ADC_CSR_CH   = 4;				//转换通道选择AIN4
	
	ADC_CR1_ADON = 1;				//打开ADC
}

/****************************************************************************
** 函数名称 ：void ADC_Init (void)
** 函数功能 ：ADC初始化
****************************************************************************/
void ADC_Init (void)
{
	ADC_IOConfig();
	ADC_RegConfig();
}

/****************************************************************************
** 函数名称 ：static uint8 ADC_TurnOn (void)
** 函数功能 ：启动ADC转换
** 返回值   ：1：操作成功  0：ADC模块正忙 
****************************************************************************/
static uint8 ADC_TurnOn (void)
{
	if (ADC1.Status != AD_Idle) return 0;	//只有ADC为空闲时才能启动转换
	
	ADC1.Status  = AD_Working;	
	ADC_CR1_ADON = 1;
	return 1;
}

/****************************************************************************
** 函数名称 ：void ADC_Process (void)
** 函数功能 ：ADC转换处理
****************************************************************************/
void ADC_Process (void)
{
	uint16 ADCTmp = (ADC1.CvtValue < ADC_CVTMIN) ? 0UL : ((ADC1.CvtValue > ADC_CVTMAX) ? 1024UL : ADC1.CvtValue);	
		
	/*采用滑动滤波算法计算ADC采样值*/	 
	ADC1.CvtSum += ADCTmp;
	ADC1.CvtSum -= ADC1.CvtRes[ADC1.CvtCnt];
	
	ADC1.CvtRes[ADC1.CvtCnt++] = ADCTmp;
	ADC1.CvtCnt = (ADC1.CvtCnt >= ADCBUF_NUM) ? 0 : ADC1.CvtCnt;
	
	ADC1.CalcRes = ADC1.CvtSum/ADCBUF_NUM;
		
	ADC_TurnOn();		
}

/****************************************************************************
** 函数名称 ：uint16 ADC_GotCvtRes (void)
** 函数功能 ：获取ADC转换结果
****************************************************************************/
uint16 ADC_GotCvtRes (void)
{
	return ADC1.CalcRes;
}

/****************************************************************************
** 函数名称 ：__interrupt void ADC_EOC_IRQHandler (void)
** 函数功能 ：ADC转换中断处理
****************************************************************************/
#pragma vector = ADC1_EOC_vector
__interrupt void ADC_EOC_IRQHandler (void)
{
	HALF_WORD Data;
	
	/*保存ADC转换结果*/
	Data.BYTE.LByte = ADC_DRL;
	Data.BYTE.HByte = ADC_DRH;
	ADC1.CvtValue   = Data.HalfWord;
		
	ADC1.Status = AD_Idle;
	ADC_CSR_EOC = 0;	
}




