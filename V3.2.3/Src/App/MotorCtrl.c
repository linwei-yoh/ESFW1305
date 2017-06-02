#include "Config.h"
#include "ADC.h"
#include "IOCtrl.h"
#include "SpdCalc.h"
#include "PwmGen.h"
#include "PI.h"
#include "MenuCtrl.h"
#include "MenuData.h"
#include "PanelCtrl.h"
#include "MotorModeCtrl.h"
#include "MotorCtrl.h"
#include "MotorParam.h"
#include "Encrypt.h"
#include "Eeprom.h"


MOTOR Motor = MOTOR_DEFAULT;
/****************************************************************************
** �������� ��void MotorCtrl_Init (void)
** �������� ��������Ƴ�ʼ��
****************************************************************************/
void MotorCtrl_Init (void)
{
	IOCtrl_Init();				//IO�ڳ�ʼ��
	ADC_Init();					//ADCģ���ʼ��  	  ����0/1024/ָ����Χ�ڵ�ֵ
	SpdCalc_Init();				//�����ٶ������ʼ��        
	PwmGen_Init();				//�ɿع败�������ʼ��
	
	Motor.Status = MIdle;
	IOCtrl_LedCtrl(WIdle);	
}

/****************************************************************************
** �������� ��static void MotorCtrl_SpeedCtrl (void)
** �������� ������Ӽ��ٿ���
****************************************************************************/
static void MotorCtrl_SpeedCtrl (void)
{  	
  	uint16 AccRate = 0;
	uint16 DecRate = 0;
	
	if((Motor.Work_Sta == WIdle) || (Motor.Work_Sta == WStop)) Motor.SpdCvt.SpdTar = 0;
	else Motor.SpdCvt.SpdTar = Motor.SpdCvt.SpdIn;
	
	//Ŀ���ٶ� > �����ٶ�
	if (Motor.SpdCvt.SpdTar > Motor.SpdCvt.RefSpd)
	{
	    //��ʱ���ۼ��ٶ�
	  	Motor.SpdCvt.AccDly += Motor.SpdCvt.AccRate;
		AccRate = Motor.SpdCvt.AccDly / 100;
 		if(AccRate != 0) Motor.SpdCvt.AccDly -= AccRate*100;
		
		Motor.SpdCvt.SpdTmp = Motor.SpdCvt.RefSpd + AccRate;
		Motor.SpdCvt.RefSpd = (Motor.SpdCvt.SpdTmp > Motor.SpdCvt.SpdTar) ? 
							  Motor.SpdCvt.SpdTar : Motor.SpdCvt.SpdTmp;
	}
	//Ŀ���ٶ� < �����ٶ�
	else if (Motor.SpdCvt.SpdTar < Motor.SpdCvt.RefSpd)
	{
		if (Motor.SpdCvt.DecRate == 0) Motor.SpdCvt.RefSpd = Motor.SpdCvt.SpdTar;
		else
		{
			Motor.SpdCvt.DecDly += Motor.SpdCvt.DecRate;
			DecRate = Motor.SpdCvt.DecDly /100;
			if(DecRate != 0) Motor.SpdCvt.DecDly -= DecRate*100;
				
			Motor.SpdCvt.SpdTmp = Motor.SpdCvt.RefSpd - DecRate;
			Motor.SpdCvt.RefSpd = (Motor.SpdCvt.SpdTmp < Motor.SpdCvt.SpdTar) ? 
								  Motor.SpdCvt.SpdTar : Motor.SpdCvt.SpdTmp;
		}
	}
}

/****************************************************************************
** �������� ��void MotorCtrl_Process (void)
** �������� ��������ƽ���
****************************************************************************/
void MotorCtrl_Process (void)
{	
	ADC_Process();				//ADCģ�鴦��
	SpdCalc_Process();			//�ٶȷ�������
	
	MotorParam_SetSpdIn();		//��������趨�ٶȻ�ȡ	 	
	Motor.SpdCvt.FdbSpd = SpdCalc_GotSpd();	//��÷����ٶ�
	
	if(Menu_Param.stPARAM_List.Rev_En.Val == 2)
		MotorMode_CycleProcess();
	else
	  	MotorMode_NormalProcess();
	
	switch(Motor.Status)
	{
		case MIdle:
			if ((Motor.Work_Sta == WFwd) || (Motor.Work_Sta == WRev))
			{
				Motor.SpdPid.i1  = 0;
				PwmGen_SetTRIGStren(0);
				PwmGen_EnableTRIGO();
				
				Motor.Status = MRun;
			}
			break;
		case MRun:
			if(((Motor.Work_Sta == WIdle) || (Motor.Work_Sta == WStop)) && (Motor.SpdCvt.RefSpd <= LOW_SPEED))
			{
				PwmGen_SetTRIGStren(0);
				PwmGen_DisableTRIGO();
		
				Encrypt_Function();	
				
				Motor.SpdCvt.RefSpd = 0;
				Motor.SpdCvt.AccDly = 0;
				Motor.SpdCvt.DecDly = 0;
				Motor.SpdPid.i1     = 0;
				Motor.LockedRTCnt   = 0;
				
				Motor.Status = MIdle;
				IOCtrl_LedCtrl(WIdle);
			}
			else
			{
				MotorCtrl_SpeedCtrl();
				Motor.SpdPid.Ref = Motor.SpdCvt.RefSpd;
				Motor.SpdPid.Fdb = Motor.SpdCvt.FdbSpd;
				PID_MACRO(Motor.SpdPid)
				
				PwmGen_SetTRIGStren(Motor.SpdPid.Out);				
					
				if(Motor_LockEnable == DISABLE) break;
				//��ת���
				if (Motor.SpdCvt.FdbSpd < LOW_SPEED)
				{
					if (Motor.LockedRTCnt < LOCKED_TIMEOUT) Motor.LockedRTCnt++;
					else 
					{
						Motor.LockedRTCnt = 0;
						Motor.Status = MErr;
						MotorParam_SetWorkSta(WIdle);
						IOCtrl_LedCtrl(WIdle);
						MotorMode_CycReSet();
					}
					Motor.BounceCnt = 0;
				}
				else
				{
					if (Motor.BounceCnt < LOCKED_BNC_TIMEOUT) Motor.BounceCnt++;
					else
					{
						Motor.BounceCnt   = 0;
						Motor.LockedRTCnt = 0;
					}
				}
			}
			break;
		case MErr:
			PwmGen_SetTRIGStren(0);
			PwmGen_DisableTRIGO();
			Cur_FocusMenu  = 4; 	//���ý��浽�������
          	Dis_SetString("Err1",0);
			break;
		default:
			break;
	}
}





