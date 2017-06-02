#include "iostm8s103f3.h"
#include "IOCtrl.h"


/*****************************************************************************
** �������ƣ�void IOCtrl_Init (void)
** ����������IO�ڳ�ʼ��
*****************************************************************************/
void IOCtrl_Init (void)
{	
	/*LED_FWD*/	 
	PD_DDR_DDR6 = 1;		//����PD6Ϊ���
	PD_CR1_C16  = 1;		//�������
	PD_CR2_C26  = 0;		//�������Ϊ2MHz
	
	/*LED_REV*/		 
	PD_DDR_DDR5 = 1;		//����PD5Ϊ���
	PD_CR1_C15  = 1;		//�������
	PD_CR2_C25  = 0;		//�������Ϊ2MHz
	
	/*LED_STOP*/	 
	PD_DDR_DDR4 = 1;		//����PD4Ϊ���
	PD_CR1_C14  = 1;		//�������
	PD_CR2_C24  = 0;		//�������Ϊ2MHz
}

/*****************************************************************************
** �������ƣ�void IOCtrl_LedCtrl (WORK_STATUS Cmd)
** ����������LED��ɫ����
*****************************************************************************/
void IOCtrl_LedCtrl (WORK_STATUS Cmd)
{
	switch(Cmd)
	{
		case WFwd:
			TRUN_ON_FWD;
			TRUN_OFF_REV;
			TRUN_OFF_STOP;
			break;
		case WRev:
			TRUN_OFF_FWD;
			TRUN_ON_REV;
			TRUN_OFF_STOP;
			break;
		case WIdle:
		case WStop:
			TRUN_OFF_FWD;
			TRUN_OFF_REV;
			TRUN_ON_STOP;
			break;
		default:
			break;
	}
}




