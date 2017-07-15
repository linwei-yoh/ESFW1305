#include "MotorModeCtrl.h"
#include "MenuData.h"
#include "PanelCtrl.h"
#include "IOCtrl.h"
#include "MotorCtrl.h"

CYCLE Cycle = CYCLE_DEFAULT;
uint16 usCyc_Spd = 0;
uint8  ucCys_Mark = 0;

/******************************************************************************
*函数名： void MotorMode_SetCmd(WORK_STATUS Sta)
*作用：   在普通运行模式时直接设定运行方式
*******************************************************************************/
void MotorMode_SetCmd(WORK_STATUS Sta)
{
 	if(Menu_Param.stPARAM_List.Control_Mode.Val == 4)  //记忆模式 读取电机状态
    {
		if((Menu_Param.stPARAM_List.Rev_En.Val == 1)) //非循环运行模式
		{
			if(Sta == WFwd) Motor_Signal = K_Fwd;
			else if(Sta == WRev) Motor_Signal = K_Rev;
            else Motor_Signal = K_Stop;
		}	
	}	
}

/**********************************************
*函数名： void MotorMode_NormalProcess(void)
*作用：   普通运行模式
***********************************************/
void MotorMode_NormalProcess(void)
{
	switch(Motor_Signal)
	{
		case K_Fwd:
			if(Menu_Param.stPARAM_List.Rotate_Mode.Val == 3)	break;
			if((Motor.Status == MIdle) || (Motor.Status == MRun && Motor.Work_Sta == WIdle))
				MotorParam_SetWorkSta(WFwd);
	  		break;
		case K_Rev:
			if(Menu_Param.stPARAM_List.Rotate_Mode.Val == 2)	break;
			if((Motor.Status == MIdle) || (Motor.Status == MRun && Motor.Work_Sta == WIdle))
			  	MotorParam_SetWorkSta(WRev); 	
	  		break;
		case K_Stop:
			if(Motor.Status == MRun)
				MotorParam_SetWorkSta(WIdle);
	  		break;
		case NoKey:
	  		break;
		default:
	  		break;
	}
	Motor_Signal = NoKey;
}

/**********************************************
*函数名： void MotorMode_CycReSet(void)
*作用：   循环运行模式重置
***********************************************/
void MotorMode_CycReSet(void)
{
	Cycle.Cyc_Sta = CIdle;
	Cycle.Round_Count = 0;
	Cycle.SecDelay = 0;
}

/**********************************************
*函数名： uint8 MotorMode_CycDelay(void)
*作用：   循环运行模式延时函数
***********************************************/
static uint8 MotorMode_CycDelay(void)
{
	if(Cycle.SecDelay++ >= 500)  //每一秒
	{
	  	Cycle.SecDelay = 0;
		Cycle.Delay--;
	}
	
	if(Cycle.Delay == 0)  return 1;
	return 0;
}

/**********************************************
*函数名： void MotorMode_CycSetSpd(uint16 CycSpd)
*作用：   设置当前循环方向上 运行速度显示
***********************************************/
void MotorMode_CycSetSpd(uint16 CycSpd)
{
	Motor.SpdCvt.SpdIn = CycSpd;
	usCyc_Spd = CycSpd;
	ucCys_Mark = 1;
}

/**********************************************
*函数名： void MotorMode_CycleProcess(void)
*作用：   循环运行模式
***********************************************/
void MotorMode_CycleProcess(void)
{
	uint16 R_Mode = Menu_Param.stPARAM_List.Rotate_Mode.Val;
	
	if((Motor_Signal == K_Stop) && ((Cycle.Cyc_Sta == CRun) || (Cycle.Cyc_Sta == CStop)))
 	{
		MotorParam_SetWorkSta(WStop);
		Cycle.Cyc_Sta = CEnd;
	}
		
	switch(Cycle.Cyc_Sta)
	{
		case CIdle://idle
		  	//运行方向使能判定
			if((R_Mode != 3) && (Menu_Param.stPARAM_List.Rev_FRTime.Val != 0))	
			  	Cycle.Fwd_Enable = 1;
			else 
			  	Cycle.Fwd_Enable = 0;
			if((R_Mode != 2) && (Menu_Param.stPARAM_List.Rev_RRTime.Val != 0))
			  	Cycle.Rev_Enable = 1;
			else
			  	Cycle.Rev_Enable = 0;
			
			if((Motor_Signal == K_Fwd) && Cycle.Fwd_Enable)
			{
			  	MotorParam_SetWorkSta(WFwd);
				MotorMode_CycSetSpd(Menu_Param.stPARAM_List.Rev_CorSpd.Val);
				Cycle.Delay = Menu_Param.stPARAM_List.Rev_FRTime.Val;
				Cycle.Cyc_Dir = 0;
			  	Cycle.Cyc_Sta = CRun;
			}
			else if((Motor_Signal == K_Rev) && Cycle.Rev_Enable)
			{
			  	MotorParam_SetWorkSta(WRev);
				MotorMode_CycSetSpd(Menu_Param.stPARAM_List.Rev_BakSpd.Val);
				Cycle.Delay = Menu_Param.stPARAM_List.Rev_RRTime.Val;
				Cycle.Cyc_Dir = 1;
				Cycle.Cyc_Sta = CRun;
			}
			break;
		case CRun:
			if(MotorMode_CycDelay() == 1)
			{				
				Cycle.Round_Count++;				
				MotorParam_SetWorkSta(WStop);
				
				//循环完成检测
				if(Cycle.Round_Count >= Menu_Param.stPARAM_List.Rev_Num.Val)
					Cycle.Cyc_Sta = CEnd;
				else
				{
					Cycle.Delay = Menu_Param.stPARAM_List.Rev_STime.Val;
					Cycle.Cyc_Sta = CStop;
				}
			}
			break;
		case CStop:
			if(Motor.Status == MIdle)
			{			    
				//循环间歇延时
				if(MotorMode_CycDelay() == 1)
				{ 
					if(Cycle.Cyc_Dir == 0 && Cycle.Rev_Enable == 1)
						Cycle.Cyc_Dir = 1;
					else if(Cycle.Cyc_Dir == 1 && Cycle.Fwd_Enable == 1)
					    Cycle.Cyc_Dir = 0;
					
					if(Cycle.Cyc_Dir == 0)
					{
					  	Cycle.Delay = Menu_Param.stPARAM_List.Rev_FRTime.Val;
					  	MotorParam_SetWorkSta(WFwd);
						MotorMode_CycSetSpd(Menu_Param.stPARAM_List.Rev_CorSpd.Val);
					}
					else
					{
					  	Cycle.Delay = Menu_Param.stPARAM_List.Rev_RRTime.Val;
					  	MotorParam_SetWorkSta(WRev);
						MotorMode_CycSetSpd(Menu_Param.stPARAM_List.Rev_BakSpd.Val);
					}
					Cycle.Cyc_Sta = CRun;
				}
			}
			break;
		case CEnd:
	  		if(Motor.Status == MIdle)
			{
			  	MotorParam_SetWorkSta(WIdle);
				Cycle.Cyc_Sta = CIdle;
				Cycle.Round_Count = 0;
				Cycle.SecDelay = 0;
			}
	  		break;
		default:
			break;
	}
	Motor_Signal = NoKey;
}

