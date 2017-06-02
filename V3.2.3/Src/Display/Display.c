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
                  ,{0x1f,0x28},{0x06,0x20},{0x1b,0x30},{0x0f,0x30},{0x06,0x38}			 //带小数点 0-4
                  ,{0x0d,0x38},{0x1d,0x38},{0x07,0x20},{0x1f,0x38},{0x0f,0x38}};		 //带小数点 5-9};
*/                          
uint8 Dis_Val[4] = {1,2,3,4};
static uint8 Point_mark = 0;
static uint8 Dis_Change  = 1;

static uint16 Fdb_Delay = 0;

//在速度反馈界面 通过按钮设定速度
static uint8 SpdChMark = 0;


/*--------------------------------------------显示内容设置--------------------------------------------------------------*/
/*********************************************************************
* 函数名称： 	void Dis_SetNum(uint16 Val,uint8 Point)
* 功能介绍：	用于显示0-9999之间的数字 大于该范围的数字 仅保留低四位
				Point 只有1-4有效 第Point位带小数点
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
	
    //从高位到低位 第一位非0或者不带小数点的0前的0 用空格取代 即不显示
    for(i=0;i<3;i++)
    {
      if((Dis_Val[i] != 0) || ((i + 1) == Point))
        break;
      else
        Dis_Val[i] = 16;
    }	
}

/*************************************************************
* 函数名称： 	void Dis_SetString(uint8 *cData,uint8 Point)
* 功能介绍：	用于显示一串最长4个字符的字符串 
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

/*-----------------------------------------------TM芯片显示指令-------------------------------------------------------------*/

/*************************************************************
* 函数名称： 	Send_Data(void)
* 功能介绍：	将要显示的内容发送给芯片
*************************************************************/							  
static void Send_Data(void)
{
    int8 i=0;	
 
	TM_STB = 0;
	
	TM_SendByte(0xc0);        //设置显示地址   11 从0x00地址开始
	
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

/*----------------------------------接口函数--------------------------------*/
/*************************************************************
* 函数名称： 	void Display_Init(void)
* 功能介绍：	系统的显示控制初始化
*************************************************************/
void Display_Init(void)
{
  	Dis_SetNum(MotorParam_GetDisSpd(),0);
}

/*************************************************************
* 函数名称： 	void Display_FdbSetSpd(void)
* 功能介绍：	在反馈速度模式下 设定速度
*************************************************************/
void Display_FdbSetSpd()
{
  	SpdChMark = 1;
	Fdb_Delay = 0;
}

/*************************************************************
* 函数名称： 	void Display_Process(void)
* 功能介绍：	系统的显示控制进程
*************************************************************/
void Display_Process(void)					
{	
	uint16 Spd_Tmp = 0;
	uint8  Dis_Mode = Menu_Param.stPARAM_List.Dis_Mode.Val;
	
	if(Cur_FocusMenu == 0)
	{
	  	//在循环开启模式下 速度显示
	  	if(Menu_Param.stPARAM_List.Rev_En.Val == 2)
		{
		  	if(ucCys_Mark == 1)
			{
				Dis_SetNum(usCyc_Spd,0);
				ucCys_Mark = 0;
			}
		}
	  	//在速度反馈模式下 每0.4秒刷新一次反馈速度显示
		else if((Dis_Mode == 3) && SpdChMark == 0)
		{
			if(Fdb_Delay++ > 200)  
			{
				Fdb_Delay = 0;
				Dis_SetNum(SpdCalc_GotDisSpd(),0);
			}
		}
		//在速度反馈模式下调节速度 
		else if((Dis_Mode == 3) && SpdChMark == 1)
		{
		    //会显示1秒 设定速度 再回到反馈速度显示
		  	if(Fdb_Delay++ > 500)
			{
			  	SpdChMark = 0;
				Fdb_Delay = 0;
			}
		}
		//ADC模式下的速度显示 每0.2S刷新一次ADC的值
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
	
    //数码管显示控制
    if(Dis_Change == 1)    		//显示内容有变化
    {
      TM_Command(0x40);         //设置数据命令   01      	 									
      Send_Data();	
      TM_Command(0x88);       	//显示与亮度控制 10	
		
      Dis_Change = 0;
    }
}





