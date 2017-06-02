#ifndef __ADC_H__
#define __ADC_H__


#include "Config.h"
/******************************************************************
						  数据类型申明
******************************************************************/
typedef enum {AD_Idle = 0, AD_Working} AD_STATUS;

typedef struct
{
	AD_STATUS Status;				//ADC模块工作状态
	uint16 CalcRes;					//ADC转换计算结果
	uint16 CvtValue;				//ADC转换结果
	uint16 CvtSum;					//转换结果累加和
	uint16 CvtRes[ADCBUF_NUM];		//转换结果缓存池
	uint8  CvtCnt;					//当前已转换数量
}ADC_STRUCT;

/******************************************************************
						  数据类型缺省值
******************************************************************/
#define ADC_STRUCT_DEFAULT	{AD_Idle, 0, 0, 0, {0}, 0}

/******************************************************************
						  接口函数申明
******************************************************************/
extern void ADC_Init (void);
extern void ADC_Process (void);
extern uint16 ADC_GotCvtRes (void);






#endif
