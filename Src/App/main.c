#include "main.h"



/*****************************************************************************
** �������ƣ�int main (void)
** ����������������
*****************************************************************************/
int main (void)
{
	uint16 StartDelay;
	
	/*�����ӳٵȴ�������������ϵ����*/
	for (StartDelay = 60000;StartDelay > 0;StartDelay--);
	
	asm("sim");  				//��ֹ�ж�
	SYSCLK_INIT();				//ʱ�ӳ�ʼ��		
	PanelCtrl_Init();			//���ģ���ʼ��
	MotorCtrl_Init();			//�������ģ���ʼ��
    MenuData_Init();
	EERPROM_Init();				//EEPROM��ʼ��
	TIM4_Init();				//��ʱ��4��ʼ��	
	asm("rim");  				//ȫ���ж�ʹ��
	
	while(1)
	{
		if (TIM4SIG)   
		{	
			MotorCtrl_Process();	//������ƴ���		
			PanelCtrl_Process();	//���ģ�鴦��
			MenuCtrl_Process();		//�˵����ƴ���		
			EEPROM_Process();		//EEPROM����
			TIM4SIG = 0;			//�����ʱ�ź�
		}
	}
}




