#ifndef __MENUDATA_H__
#define __MENUDATA_H__


#include "type.h"

typedef struct
{
	uint16 Min;
	uint16 Max;
}V_RANGE;

typedef struct
{
    uint16      Val;
    V_RANGE  	*Range;
}PARAM_VAL;


typedef struct 
{
	PARAM_VAL Cur_Speed;                        //0  ����趨�ٶ�
	PARAM_VAL Dis_Mode;                         //1  ��ֵ��ʾ��ʽ 1��ֱ����ʾ  2. ������ʾ  3.������ʾ
	PARAM_VAL Ratio;                            //2  ��������  
	PARAM_VAL Control_Mode;              	    //3  ���п��� 1:������/�޼��� 2:��ӿ���/STOP��Ч 3:��ӿ���/STOP��Ч 4:������/����
	PARAM_VAL Rotate_Mode;		        		//4  ��ת��ʽ 1:��������ת 2:����ת 3:����ת
	PARAM_VAL Rotate_Dir;			        	//5  ��ת���� 1:��ȡ�� 2:ȡ��
	PARAM_VAL SpeedCtrl_Mode;	                //6  �ٶȵ��� 1:������¼����� 2:�����ť����
	PARAM_VAL Max_Speed;                        //7  ���ת��
	PARAM_VAL Min_Speed;                        //8  ���ת��
	PARAM_VAL Cor_BTime;			        	//9  ��ת��������ʱ�� 1-100��0.01S��										                       
	PARAM_VAL Cor_Mode;			        		//10 ��תֹͣ��ʽ 1:���ɼ���ֹͣ 2:��������ֹͣ										             
	PARAM_VAL Cor_ETime;			        	//11 ��תֹͣʱ��������ʱ��  1-100��0.01S��										                
	PARAM_VAL Bak_BTime;			        	//12 ��ת������������ 1-100��0.01S��													   							 
	PARAM_VAL Bak_Mode;			        		//13 ��תֹͣ��ʽ	1:���ɼ���ֹͣ 2:��������ֹͣ                         		 
	PARAM_VAL Bak_ETime;			        	//14 ��תֹͣʱ��������ʱ��  1-100��0.01S��	
	PARAM_VAL Err_Report;                       //15 ���ϱ���ʹ��    1.��Ч 2��Ч
	PARAM_VAL Rev_En;                           //16 ѭ������ʹ��  1.��Ч 2ʹ��  
	PARAM_VAL Rev_Num;                          //17 ���д��� 1-9999
	PARAM_VAL Rev_FRTime;                       //18 ����������ʱ�� (s  0-9999
	PARAM_VAL Rev_RRTime;                       //19 ����������ʱ�� (s  0-9999
	PARAM_VAL Rev_STime;                        //20 �м�ֹͣʱ��   (s  1-9999
	PARAM_VAL Rev_CorSpd;						//21 �����������ٶ� (90-3000
	PARAM_VAL Rev_BakSpd;						//22 �����������ٶ� (90-3000
	PARAM_VAL ReSet;			        		//23 �ָ������趨  1:���ָ� 2:�ָ������趨
	PARAM_VAL SpdFdb;                           //24 �ٶȷ����ش�С
}MUME_LIST;

#define LIST_Size (sizeof(MUME_LIST) / sizeof(PARAM_VAL))

typedef union
{
  MUME_LIST  stPARAM_List;
  PARAM_VAL  stList[LIST_Size];
}MENU_PARAM;

extern int16 Radio_Spd ;
extern MENU_PARAM Menu_Param;
extern void MenuData_Init(void);

#endif
