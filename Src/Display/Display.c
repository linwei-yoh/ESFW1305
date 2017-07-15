#include "Display.h"
#include "TM1618.h"
#include "MenuData.h"
#include "MenuCtrl.h"
#include "SpdCalc.h"
#include "ADC.h"
#include "MotorCtrl.h"
#include "MotorParam.h"
#include "MotorModeCtrl.h"

uint8 code[20][2]={{0x1f,0x08},{0x06,0x00},{0x1b,0x10},{0x0f,0x10},{0x06,0x18}			 //0-4
                  ,{0x0d,0x18},{0x1d,0x18},{0x07,0x00},{0x1f,0x18},{0x0f,0x18}			 //5-9
                  ,{0x17,0x18},{0x1c,0x18},{0x19,0x08},{0x1e,0x10},{0x19,0x18}			 //A-E
                  ,{0x11,0x18},{0x00,0x00},{0x10,0x10},{0x14,0x10},{0x00,0x10}};      	 //F ' ' 'r' 'n' '-'  
/*
                  ,{0x1f,0x28},{0x06,0x20},{0x1b,0x30},{0x0f,0x30},{0x06,0x38}			 //��С���� 0-4
                  ,{0x0d,0x38},{0x1d,0x38},{0x07,0x20},{0x1f,0x38},{0x0f,0x38}};		 //��С���� 5-9};
*/                          
uint8 Dis_Val[4] = {1,2,3,4};
static uint8 Point_mark = 0;
static uint8 Dis_Change  = 1;

static uint16 Fdb_Delay = 0;

//���ٶȷ������� ͨ����ť�趨�ٶ�
static uint8 SpdChMark = 0;


/*--------------------------------------------��ʾ��������--------------------------------------------------------------*/
/*********************************************************************
* �������ƣ� 	void Dis_SetNum(uint16 Val,uint8 Point)
* ���ܽ��ܣ�	������ʾ0-9999֮������� ���ڸ÷�Χ������ ����������λ
				Point ֻ��1-4��Ч ��Pointλ��С����
**********************************************************************/
void Dis_SetNum(uint16 Val,uint8 Point)
{
    int8 i=0;
    uint16 Tmp_Val;
	
    Dis_Change = 1;
    Tmp_Val    = (Val>=10000)?(Val-10000):Val;
    
	Dis_Val[0] = Tmp_Val / 1000;
	Dis_Val[1] = (Tmp_Val % 1000)/100;
	Dis_Val[2] = (Tmp_Val % 100)/10;
	Dis_Val[3] = Tmp_Val % 10;
	
	Point_mark = Point;
	
    //�Ӹ�λ����λ ��һλ��0���߲���С�����0ǰ��0 �ÿո�ȡ�� ������ʾ
    for(i=0;i<3;i++)
    {
      if((Dis_Val[i] != 0) || ((i + 1) == Point))
        break;
      else
        Dis_Val[i] = 16;
    }	
}

/*************************************************************
* �������ƣ� 	void Dis_SetString(uint8 *cData,uint8 Point)
* ���ܽ��ܣ�	������ʾһ���4���ַ����ַ��� 
*************************************************************/
void Dis_SetString(uint8 *cData,uint8 Point)
{
    uint8 i=0;
    uint8 temp=0;
	
    Dis_Change = 1;
	Point_mark = Point;
    for(i=0;i<4;i++)
    {
      temp = *(cData+i);
      if(temp >= '0' && temp <= '9')
          Dis_Val[i] = temp - '0';
      else if(temp >= 'a' && temp <= 'f')
          Dis_Val[i] = temp - 'a' + 10;
      else if(temp >= 'A' && temp <= 'F')
          Dis_Val[i] = temp - 'A' + 10;
      else if(temp == ' ')
          Dis_Val[i] = 16;
      else if(temp == 'r')
          Dis_Val[i] = 17;
      else if(temp == 'n')
          Dis_Val[i] = 18;
      else if(temp == '-')
          Dis_Val[i] = 19;
    }
}

/*-----------------------------------------------TMоƬ��ʾָ��-------------------------------------------------------------*/

/*************************************************************
* �������ƣ� 	Send_Data(void)
* ���ܽ��ܣ�	��Ҫ��ʾ�����ݷ��͸�оƬ
*************************************************************/							  
static void Send_Data(void)
{
    int8 i=0;	
 
	TM_STB = 0;
	
	TM_SendByte(0xc0);        //������ʾ��ַ   11 ��0x00��ַ��ʼ
	
    for(i=0;i<4;i++)	           
    {							 
      TM_SendByte(code[Dis_Val[i]][0]);		
	  
	  if(Point_mark == (i+1))	  
		TM_SendByte(code[Dis_Val[i]][1] | 0x20);
	  else
      	TM_SendByte(code[Dis_Val[i]][1]);	
    }	
	Point_mark = 0;
	
	TM_STB = 1;
} 

/*----------------------------------�ӿں���--------------------------------*/
/*************************************************************
* �������ƣ� 	void Display_Init(void)
* ���ܽ��ܣ�	ϵͳ����ʾ���Ƴ�ʼ��
*************************************************************/
void Display_Init(void)
{
  	Dis_SetNum(MotorParam_GetDisSpd(),0);
}

/*************************************************************
* �������ƣ� 	void Display_FdbSetSpd(void)
* ���ܽ��ܣ�	�ڷ����ٶ�ģʽ�� �趨�ٶ�
*************************************************************/
void Display_FdbSetSpd()
{
  	SpdChMark = 1;
	Fdb_Delay = 0;
}

/*************************************************************
* �������ƣ� 	void Display_Process(void)
* ���ܽ��ܣ�	ϵͳ����ʾ���ƽ���
*************************************************************/
void Display_Process(void)					
{	
	uint16 Spd_Tmp = 0;
	uint8  Dis_Mode = Menu_Param.stPARAM_List.Dis_Mode.Val;
	
	if(Cur_FocusMenu == 0)
	{
	  	//��ѭ������ģʽ�� �ٶ���ʾ
	  	if(Menu_Param.stPARAM_List.Rev_En.Val == 2)
		{
		  	if(ucCys_Mark == 1)
			{
				Dis_SetNum(usCyc_Spd,0);
				ucCys_Mark = 0;
			}
		}
	  	//���ٶȷ���ģʽ�� ÿ0.4��ˢ��һ�η����ٶ���ʾ
		else if((Dis_Mode == 3) && SpdChMark == 0)
		{
			if(Fdb_Delay++ > 200)  
			{
				Fdb_Delay = 0;
				Dis_SetNum(SpdCalc_GotDisSpd(),0);
			}
		}
		//���ٶȷ���ģʽ�µ����ٶ� 
		else if((Dis_Mode == 3) && SpdChMark == 1)
		{
		    //����ʾ1�� �趨�ٶ� �ٻص������ٶ���ʾ
		  	if(Fdb_Delay++ > 500)
			{
			  	SpdChMark = 0;
				Fdb_Delay = 0;
			}
		}
		//ADCģʽ�µ��ٶ���ʾ ÿ0.2Sˢ��һ��ADC��ֵ
		else if(Menu_Param.stPARAM_List.SpeedCtrl_Mode.Val == 2)
		{
			if(Fdb_Delay++ > 100)  
			{
				Fdb_Delay = 0;			
				Spd_Tmp = Motor.SpdCvt.SpdIn;

				if(Dis_Mode == 1)
					Dis_SetNum(Spd_Tmp,0);
				else if(Dis_Mode == 2)
					Dis_SetNum(Spd_Tmp*10/Menu_Param.stPARAM_List.Ratio.Val,0);
			}
		}
	}
	
    //�������ʾ����
    if(Dis_Change == 1)    		//��ʾ�����б仯
    {
      TM_Command(0x40);         //������������   01      	 									
      Send_Data();	
      TM_Command(0x88);       	//��ʾ�����ȿ��� 10	
		
      Dis_Change = 0;
    }
}





