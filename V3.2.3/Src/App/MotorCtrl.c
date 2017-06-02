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
** 函数名称 ：void MotorCtrl_Init (void)
** 函数功能 ：电机控制初始化
****************************************************************************/
void MotorCtrl_Init (void)
{
	IOCtrl_Init();				//IO口初始化
	ADC_Init();					//ADC模块初始化  	  返回0/1024/指定范围内的值
	SpdCalc_Init();				//反馈速度运算初始化        
	PwmGen_Init();				//可控硅触发输出初始化
	
	Motor.Status = MIdle;
	IOCtrl_LedCtrl(WIdle);	
}

/****************************************************************************
** 函数名称 ：static void MotorCtrl_SpeedCtrl (void)
** 函数功能 ：电机加减速控制
****************************************************************************/
static void MotorCtrl_SpeedCtrl (void)
{  	
  	uint16 AccRate = 0;
	uint16 DecRate = 0;
	
	if((Motor.Work_Sta == WIdle) || (Motor.Work_Sta == WStop)) Motor.SpdCvt.SpdTar = 0;
	else Motor.SpdCvt.SpdTar = Motor.SpdCvt.SpdIn;
	
	//目标速度 > 给定速度
	if (Motor.SpdCvt.SpdTar > Motor.SpdCvt.RefSpd)
	{
	    //按时间累加速度
	  	Motor.SpdCvt.AccDly += Motor.SpdCvt.AccRate;
		AccRate = Motor.SpdCvt.AccDly / 100;
 		if(AccRate != 0) Motor.SpdCvt.AccDly -= AccRate*100;
		
		Motor.SpdCvt.SpdTmp = Motor.SpdCvt.RefSpd + AccRate;
		Motor.SpdCvt.RefSpd = (Motor.SpdCvt.SpdTmp > Motor.SpdCvt.SpdTar) ? 
							  Motor.SpdCvt.SpdTar : Motor.SpdCvt.SpdTmp;
	}
	//目标速度 < 给定速度
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
** 函数名称 ：void MotorCtrl_Process (void)
** 函数功能 ：电机控制进程
****************************************************************************/
void MotorCtrl_Process (void)
{	
	ADC_Process();				//ADC模块处理
	SpdCalc_Process();			//速度反馈计算
	
	MotorParam_SetSpdIn();		//电机运行设定速度获取	 	
	Motor.SpdCvt.FdbSpd = SpdCalc_GotSpd();	//获得反馈速度
	
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
				//堵转检测
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
			Cur_FocusMenu  = 4; 	//设置界面到报错界面
          	Dis_SetString("Err1",0);
			break;
		default:
			break;
	}
}





