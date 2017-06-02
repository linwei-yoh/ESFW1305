#ifndef __ADC_H__
#define __ADC_H__


#include "Config.h"
/******************************************************************
						  ������������
******************************************************************/
typedef enum {AD_Idle = 0, AD_Working} AD_STATUS;

typedef struct
{
	AD_STATUS Status;				//ADCģ�鹤��״̬
	uint16 CalcRes;					//ADCת��������
	uint16 CvtValue;				//ADCת�����
	uint16 CvtSum;					//ת������ۼӺ�
	uint16 CvtRes[ADCBUF_NUM];		//ת����������
	uint8  CvtCnt;					//��ǰ��ת������
}ADC_STRUCT;

/******************************************************************
						  ��������ȱʡֵ
******************************************************************/
#define ADC_STRUCT_DEFAULT	{AD_Idle, 0, 0, 0, {0}, 0}

/******************************************************************
						  �ӿں�������
******************************************************************/
extern void ADC_Init (void);
extern void ADC_Process (void);
extern uint16 ADC_GotCvtRes (void);






#endif
