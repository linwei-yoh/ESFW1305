#include "MotorParam.h"
#include "MenuData.h"
#include "MotorCtrl.h"
#include "ADC.h"
#include "IOCtrl.h"
#include "PwmGen.h"
#include "Eeprom.h"
#include "SpdCalc.h"
#include <stdlib.h>

BOOL Motor_LockEnable = ENABLE;
/******************************************************************************
*��������uint16 MotorParam_GetDisSpd(void)
*���ã�  ��ȡ��ʾ���ٶ�ֵ
*******************************************************************************/
uint16 MotorParam_GetDisSpd(void)
{
	uint16 Dis_Mode = Menu_Param.stPARAM_List.Dis_Mode.Val;
	uint16 Spd_Val = Menu_Param.stPARAM_List.Cur_Speed.Val;
	uint16 Spd_Mode = Menu_Param.stPARAM_List.SpeedCtrl_Mode.Val;
	uint16 Spd_Tmp = Motor.SpdCvt.SpdIn;
	  
	if(Dis_Mode == 1)                    //ֱ����ʾ
	{
	  	if(Spd_Mode == 1)
			return Spd_Val;
		else
		  	return Spd_Tmp;
	}
	else if(Dis_Mode == 2)               //������ʾ
	{
	  	if(Spd_Mode == 1)
	 		return (uint16)Radio_Spd;
		else
		  	return Spd_Tmp*10/Menu_Param.stPARAM_List.Ratio.Val;
	}
	else 
		return SpdCalc_GotSpd();         //�����ٶ�
}

/****************************************************************************
** �������� ��void MotorParam_SetSpdIn (void)
** �������� ���趨��������ٶ�
****************************************************************************/
void MotorParam_SetSpdIn(void)
{
  	uint8 Spd_Mode = Menu_Param.stPARAM_List.SpeedCtrl_Mode.Val;
	uint32 Range = 0;
 	uint16 SpdTmp = 0;
	
	//��ѭ��ģʽ�²��ڴ˴������ٶ�
	if(Menu_Param.stPARAM_List.Rev_En.Val == 2) return;
	
	//������ٶȿ��Ʒ�ʽ
	if(Spd_Mode == 1)
	  	Motor.SpdCvt.SpdIn = Menu_Param.stPARAM_List.Cur_Speed.Val;
	
	//��ťADC�ٶȿ��Ʒ�ʽ
	else if(Spd_Mode == 2)
	{
		Range = Menu_Param.stPARAM_List.Max_Speed.Val - Menu_Param.stPARAM_List.Min_Speed.Val;
		Range = (Range * ADC_GotCvtRes())/1024UL;
		
		SpdTmp = Menu_Param.stPARAM_List.Min_Speed.Val + (uint16)Range;		
		if (abs(SpdTmp - Motor.SpdCvt.SpdIn) >= 5)
 			Motor.SpdCvt.SpdIn = SpdTmp - SpdTmp % 5;
	}
}

/****************************************************************************
** �������� ��void MotorParam_SetDir(uint8 Dir)
** �������� ���趨������з��� Dir == 0 FWD  Dir == 1 REV
****************************************************************************/
void MotorParam_SetDir(MOTOR_DIR Dir)
{
  	//����ȡ��
  	uint16 R_Dir = Menu_Param.stPARAM_List.Rotate_Dir.Val;
	
	if(R_Dir == 1)	
		PwmGen_SetTRIGDir(Dir);
	else
		PwmGen_SetTRIGDir(!Dir);
	
	//�趨��/��ת ��ͣ�ļӼ���
	if(Dir == DIR_FWD)
	{
		Motor.SpdCvt.AccRate = Menu_Param.stPARAM_List.Max_Speed.Val / Menu_Param.stPARAM_List.Cor_BTime.Val * 2;
		
		if(Menu_Param.stPARAM_List.Cor_Mode.Val == 1)
			Motor.SpdCvt.DecRate = 0;
		else
			Motor.SpdCvt.DecRate = Menu_Param.stPARAM_List.Max_Speed.Val /Menu_Param.stPARAM_List.Cor_ETime.Val * 2; 
	}
	else
	{
	  	Motor.SpdCvt.AccRate = Menu_Param.stPARAM_List.Max_Speed.Val / Menu_Param.stPARAM_List.Bak_BTime.Val * 2;
		if(Menu_Param.stPARAM_List.Bak_Mode.Val == 1)
			Motor.SpdCvt.DecRate = 0;
		else
			Motor.SpdCvt.DecRate = Menu_Param.stPARAM_List.Max_Speed.Val / Menu_Param.stPARAM_List.Bak_ETime.Val * 2; 
	}
}

/****************************************************************************
** �������� ��void MotorParam_LockCtrl(BOOL Flag)
** �������� ���趨�����ת����ʹ��
****************************************************************************/
void MotorParam_LockCtrl(BOOL Flag)
{
	Motor_LockEnable = Flag;
	Motor.LockedRTCnt = 0;
	Motor.BounceCnt   = 0;  
}

/****************************************************************************
** �������� ��void MotorParam_SetWorkSta(WORK_STATUS Sta)
** �������� ���趨������з���
****************************************************************************/
void MotorParam_SetWorkSta(WORK_STATUS Sta)
{
	Motor.Work_Sta = Sta;  
	
	if(Sta != WIdle)
		Motor.IsWorking = 1;
	else
	  	Motor.IsWorking = 0;
	
	if(Sta == WFwd)
	{
        MotorParam_SetDir(DIR_FWD);
		IOCtrl_LedCtrl(WFwd);
	}
	else if(Sta == WRev)
	{
	    MotorParam_SetDir(DIR_REV);
		IOCtrl_LedCtrl(WRev);
	}
    
    if(Menu_Param.stPARAM_List.Control_Mode.Val == 4)
        if(Menu_Param.stPARAM_List.Rev_En.Val == 1)
            EEPROM_ChMark();
}






