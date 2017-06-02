#ifndef __PWMGEN_H__
#define __PWMGEN_H__


#include "type.h"
#include "Config.h"
/******************************************************************
						    �궨��
******************************************************************/
#define TIM2_ARR_VALUE		(uint16)((Fmaster/16UL)*0.01L)		//��ʱ����ʼ���ʱ�� 10ms

#define EN_TRIGO			1
#define DIS_TRIGO			0

/******************************************************************
						  �ӿں�������
******************************************************************/
extern void PwmGen_Init (void);
extern void PwmGen_EnableTRIGO (void);
extern void PwmGen_DisableTRIGO (void);
extern uint8 PwmGen_GotTRIGSW (void);
extern void PwmGen_SetTRIGDir (uint8 Dir);
extern void PwmGen_SetTRIGStren (int32 Stren);
extern void PwmGen_GenTRIGO (void);





#endif
