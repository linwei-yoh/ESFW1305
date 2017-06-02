#include "MenuCtrl.h"
#include "MenuData.h"
#include "PanelCtrl.h"
#include "Eeprom.h"
#include "MotorCtrl.h"
#include "MotorParam.h"
#include "Display.h"
#include "MotorModeCtrl.h"

//当前菜单ID 0:正常菜单  1: 参数选择菜单 2:参数调整菜单 3:提示界面 4:故障报警界面
int8 Cur_FocusMenu  = 0;  
//参数列表   数值对应 参数选择项  参数列表从1 开始 到(LIST_Size - 1)结束 
int8 Cur_FocusParam = 1;  

//进入参数调整界面时 先将该参数项的数值 保存到该变量中
static int16 Temp_Param   = 0;  
//进入提示界面标志位
uint8 Msg_Mark = 0;
uint16 Msg_Delay = 0;

#define LONG_KEY_Val 10

 
void menu_Disable(uint8 Key_Mode);

void menu_0_Mode(uint8 Key_Mode);
void menu_0_Up(uint8 Key_Mode);
void menu_0_Down(uint8 Key_Mode);

void menu_1_Mode(uint8 Key_Mode);
void menu_1_Enter(uint8 Key_Mode);
void menu_1_Up(uint8 Key_Mode);
void menu_1_Down(uint8 Key_Mode);

void menu_2_Mode(uint8 Key_Mode);
void menu_2_Enter(uint8 Key_Mode);
void menu_2_Up(uint8 Key_Mode);
void menu_2_Down(uint8 Key_Mode);

void menu_E_Enter(uint8 Key_Mode);

SYSMENU SysMenu_Menu[5]={
                          {menu_0_Mode,menu_Disable,menu_0_Up,menu_0_Down}, 		//速度显示界面
                          {menu_1_Mode,menu_1_Enter,menu_1_Up,menu_1_Down},  		//参数项选择界面
                          {menu_2_Mode,menu_2_Enter,menu_2_Up,menu_2_Down},  		//参数设定界面
                          {menu_Disable,menu_Disable,menu_Disable,menu_Disable},   	//参数操作提示界面
                          {menu_Disable,menu_E_Enter,menu_Disable,menu_Disable}     //报错界面
                        };


/*------------------------------菜单操作函数---------------------------------*/

/******************************************************************************
*函数名： 	void Menu_GetRadioSpdVal(void)
*函数说明	在切换到比率显示模式时 修正当前设定速度
*******************************************************************************/
void Menu_GetRadioSpdVal(void)
{
	uint16 Spd_Val = Menu_Param.stPARAM_List.Cur_Speed.Val * 10;
	uint16 Spd_Rio = Menu_Param.stPARAM_List.Ratio.Val; 

	Radio_Spd = (Spd_Val + Spd_Rio - 1)/ Spd_Rio;
	Menu_Param.stPARAM_List.Cur_Speed.Val = Radio_Spd * Spd_Rio / 10;
	
	if(Menu_Param.stPARAM_List.Cur_Speed.Val > Menu_Param.stPARAM_List.Max_Speed.Val)
		Menu_Param.stPARAM_List.Cur_Speed.Val = Menu_Param.stPARAM_List.Max_Speed.Val;
}

/******************************************************************************
*函数名： uint16 Menu_SpdUpWithRio(uint8 Key_Mode)
*参数：	  Key_Mode 按键状态
*******************************************************************************/
uint16 Menu_SpdUpWithRio(uint8 Key_Mode)
{
	uint16 Spd_Max = Menu_Param.stPARAM_List.Max_Speed.Val * 10;
	uint16 Spd_Rio = Menu_Param.stPARAM_List.Ratio.Val; 	 	
	
	if(Key_Mode == KEY_UP) 				Radio_Spd++;
	else if(Key_Mode == KEY_LDOWN)   	Radio_Spd += LONG_KEY_Val;
	else return Radio_Spd;
	
	Spd_Max = (Spd_Max + Spd_Rio - 1)/Spd_Rio;
	if(Spd_Max == 1) 
	{
		Radio_Spd = 1;
		Menu_Param.stPARAM_List.Cur_Speed.Val = Menu_Param.stPARAM_List.Max_Speed.Val;
		return Radio_Spd;
	}
	
	if(Radio_Spd >= Spd_Max) 
	{
	  	Radio_Spd = Spd_Max;
	  	Menu_Param.stPARAM_List.Cur_Speed.Val = Menu_Param.stPARAM_List.Max_Speed.Val;
	}
	else
 	{
	  	Menu_Param.stPARAM_List.Cur_Speed.Val = Radio_Spd * Spd_Rio /10;
	
		if(Menu_Param.stPARAM_List.Cur_Speed.Val > Menu_Param.stPARAM_List.Max_Speed.Val)
			Menu_Param.stPARAM_List.Cur_Speed.Val = Menu_Param.stPARAM_List.Max_Speed.Val;
	}
	
	
	return Radio_Spd;
}

/******************************************************************************
*函数名： uint16 Menu_SpdDownWithRio(uint8 Key_Mode)
*参数：	  Key_Mode 按键状态
*******************************************************************************/
uint16 Menu_SpdDownWithRio(uint8 Key_Mode)
{
	uint16 Spd_Min = Menu_Param.stPARAM_List.Min_Speed.Val * 10;
	uint16 Spd_Rio = Menu_Param.stPARAM_List.Ratio.Val; 	 	
	
	if(Key_Mode == KEY_UP) 				Radio_Spd--;
	else if(Key_Mode == KEY_LDOWN)   	Radio_Spd -= LONG_KEY_Val;
	else return Radio_Spd;
	
	if(Radio_Spd < 0) Radio_Spd = 0;
	Spd_Min = (Spd_Min + Spd_Rio - 1)/Spd_Rio;
	
	if(Radio_Spd <= Spd_Min) 
	{
	  	Radio_Spd = Spd_Min;
	  	Menu_Param.stPARAM_List.Cur_Speed.Val = Menu_Param.stPARAM_List.Min_Speed.Val;
	}
	else
 	{
	  	Menu_Param.stPARAM_List.Cur_Speed.Val = Radio_Spd * Spd_Rio /10;
		
		if(Menu_Param.stPARAM_List.Cur_Speed.Val > Menu_Param.stPARAM_List.Max_Speed.Val)
			Menu_Param.stPARAM_List.Cur_Speed.Val = Menu_Param.stPARAM_List.Max_Speed.Val;
	}
	
	return Radio_Spd;
}

/******************************************************************************
*函数名： Menu_ParaUp
*参数：	  Max 最大范围 Val 当前值 Key_Mode 按键状态
*作用：   各界面参数增加调用的函数
*******************************************************************************/
uint16 Menu_ParaUp(int16 Max,int16 Val,uint8 Key_Mode)
{
    if(Key_Mode == KEY_UP)  //按键按下松开
        Val++;
    else if(Key_Mode == KEY_LDOWN) //按键长按按下
        Val += LONG_KEY_Val;
    
    if(Val > Max)
        Val = Max;
     
    return Val;
}

/******************************************************************************
*函数名： Menu_ParaDown
*参数：	  Min 最小范围 Val 当前值 Key_Mode 按键状态
*作用：   各界面参数减少调用的函数
*******************************************************************************/
uint16 Menu_ParaDown(int16 Min,int16 Val,uint8 Key_Mode)
{
    if(Key_Mode == KEY_UP)  //按键按下松开
        Val--;
    else if(Key_Mode == KEY_LDOWN) //按键长按按下
        Val -= LONG_KEY_Val; 
    
    if(Val < Min)
        Val = Min;
    
    return Val;
}

/*************************************************************
* 函数名称：void menu_Disable(uint8 Key_Mode) {}
* 功能介绍：空操作
*************************************************************/
void menu_Disable(uint8 Key_Mode) {}


/******************************************************************************
*函数名： menu_0_Mode(uint8 Key_Mode)
*作用：   在速度显示界面下Mode键调用函数
*******************************************************************************/
void menu_0_Mode(uint8 Key_Mode)
{
  if(Key_Mode == KEY_LDOWN)
  {
    Cur_FocusMenu  = 1;  //切换到参数选择菜单
    Cur_FocusParam = 1;  //参数选择编号
    Dis_SetString("F-01",0);
  }	
}

/******************************************************************************
*函数名： menu_0_Up(uint8 Key_Mode)
*作用：   在速度控制界面下Up键调用函数
*******************************************************************************/
void menu_0_Up(uint8 Key_Mode)
{
  	uint16 Dis_Mode = Menu_Param.stPARAM_List.Dis_Mode.Val;
	uint16 NewSpd = 0;
	
	if(Key_Mode != KEY_UP && Key_Mode !=  KEY_LDOWN) return;
	if(Menu_Param.stPARAM_List.Rev_En.Val == 2) return; //循环模式启动 则屏蔽
	
	//面板采用按键控制
	if(Menu_Param.stPARAM_List.SpeedCtrl_Mode.Val == 1)  
  	{
	  	NewSpd = Menu_Param.stPARAM_List.Cur_Speed.Val;
		//正常显示模式
		if(Dis_Mode == 1)
		{
			NewSpd = Menu_ParaUp(Menu_Param.stPARAM_List.Max_Speed.Val,NewSpd,Key_Mode);
			Menu_Param.stPARAM_List.Cur_Speed.Val = NewSpd;
			Dis_SetNum(NewSpd,0);			
		}
		//比例显示模式
		else if(Dis_Mode == 2)
			Dis_SetNum(Menu_SpdUpWithRio(Key_Mode),0);
		//反馈显示模式
		else if(Dis_Mode == 3)
		{
		  	NewSpd = Menu_ParaUp(Menu_Param.stPARAM_List.Max_Speed.Val,NewSpd,Key_Mode);
			Menu_Param.stPARAM_List.Cur_Speed.Val = NewSpd;
			Display_FdbSetSpd(); //设定速度显示
			Dis_SetNum(NewSpd,0);	
		}
		EEPROM_ChMark();
  	}	
}

/******************************************************************************
*函数名： menu_0_Down(uint8 Key_Mode)
*作用：   在速度控制界面下Mode键调用函数
*******************************************************************************/
void menu_0_Down(uint8 Key_Mode)
{
  	uint16 Dis_Mode = Menu_Param.stPARAM_List.Dis_Mode.Val;
	uint16 NewSpd = 0;
	
	if(Key_Mode != KEY_UP && Key_Mode !=  KEY_LDOWN) return;
	if(Menu_Param.stPARAM_List.Rev_En.Val == 2) return; //循环模式启动 则屏蔽
	
	//面板采用按键控制
	if(Menu_Param.stPARAM_List.SpeedCtrl_Mode.Val == 1)  
  	{
	  	NewSpd = Menu_Param.stPARAM_List.Cur_Speed.Val;
		//正常显示模式
		if(Dis_Mode == 1)
		{
			NewSpd = Menu_ParaDown(Menu_Param.stPARAM_List.Min_Speed.Val,NewSpd,Key_Mode);
			Menu_Param.stPARAM_List.Cur_Speed.Val = NewSpd;
			Dis_SetNum(NewSpd,0);			
		}
		//比例显示模式
		else if(Dis_Mode == 2)
			Dis_SetNum(Menu_SpdDownWithRio(Key_Mode),0);
		//反馈显示模式
		else if(Dis_Mode == 3)
		{
		  	NewSpd = Menu_ParaDown(Menu_Param.stPARAM_List.Min_Speed.Val,NewSpd,Key_Mode);
			Menu_Param.stPARAM_List.Cur_Speed.Val = NewSpd;
			Display_FdbSetSpd();
			Dis_SetNum(NewSpd,0);	
		}		
		EEPROM_ChMark();
	}
}

/******************************************************************************
*函数名： menu_1_Mode(uint8 Key_Mode)
*作用：   在功能选择界面下Mode键调用函数
*******************************************************************************/
void menu_1_Mode(uint8 Key_Mode)
{
	if(Key_Mode == KEY_UP)
  	{
		Cur_FocusMenu  = 0;   
      	Cur_FocusParam = 0;
		if(Menu_Param.stPARAM_List.Rev_En.Val == 1)
      		Dis_SetNum(MotorParam_GetDisSpd(),0);
		else
		  	ucCys_Mark = 1;
  	}
}

/******************************************************************************
*函数名： menu_1_Enter(uint8 Key_Mode)
*作用：   在功能选择界面下Enter键调用函数
*******************************************************************************/
void menu_1_Enter(uint8 Key_Mode)
{
  	MUME_LIST *List = &Menu_Param.stPARAM_List;  
	
  	if(Key_Mode == KEY_UP || Key_Mode == KEY_LUP)
  	{
    	Cur_FocusMenu = 2;
		Temp_Param = Menu_Param.stList[Cur_FocusParam].Val;            //保存原参数  
		if(Cur_FocusParam == 21 || Cur_FocusParam == 22)
		{
			List->Rev_CorSpd.Range->Max = List->Max_Speed.Val;
			List->Rev_CorSpd.Range->Min = List->Min_Speed.Val;
			List->Rev_BakSpd.Range->Max = List->Max_Speed.Val;
			List->Rev_BakSpd.Range->Min = List->Min_Speed.Val;
		}    	  		  
	
		if(Cur_FocusParam == 2 || Cur_FocusParam == 9 || Cur_FocusParam == 11 || Cur_FocusParam == 12 || Cur_FocusParam == 14)
    		Dis_SetNum(Temp_Param,3);
    	else 
	    	Dis_SetNum(Temp_Param,0);
  	}
}

/******************************************************************************
*函数名： ID_ReFresh(void)
*作用：   刷新功能选择菜单编号
*******************************************************************************/
void ID_ReFresh(void)
{
  uint8 Tmp[4] = {'F','-',' ',' '};  //F '-' '' ''
  
  Cur_FocusMenu = 1;
  Tmp[2] = '0' + Cur_FocusParam/10;   
  Tmp[3] = '0' + Cur_FocusParam%10;
  Dis_SetString(Tmp,0);
}

/******************************************************************************
*函数名： menu_1_Up(uint8 Key_Mode)
*作用：   在功能选择界面下UP键调用函数
*******************************************************************************/
void menu_1_Up(uint8 Key_Mode)
{
  if(Key_Mode == KEY_UP)
  {
    Cur_FocusParam++;

    if(Cur_FocusParam >= LIST_Size)
      Cur_FocusParam = 1;

    ID_ReFresh();
  }
}

/******************************************************************************
*函数名： menu_1_Down(uint8 Key_Mode)
*作用：   在功能选择界面下Down键调用函数
*******************************************************************************/
void menu_1_Down(uint8 Key_Mode)
{
  if(Key_Mode == KEY_UP)
  {
    Cur_FocusParam--;

    if(Cur_FocusParam < 1)
      Cur_FocusParam = LIST_Size-1;

    ID_ReFresh();
  }	
}

/******************************************************************************
*函数名： menu2OnMode(uint8 Key_Mode)
*作用：   在参数界面下Mode键调用函数
*******************************************************************************/
void menu_2_Mode(uint8 Key_Mode)
{
  if(Key_Mode == KEY_UP || Key_Mode == KEY_LUP)
    ID_ReFresh();
}

/******************************************************************************
*函数名： static uint16 menu_ChParam(void)
*作用：   参数修改检测
*******************************************************************************/
static uint16 menu_ChParam(void)
{
  //在运行状态下 不允许修改的参数项 按下确定 则设定错误提示
  if(Cur_FocusParam == 4 ||Cur_FocusParam == 5 || Cur_FocusParam == 7 
      ||Cur_FocusParam == 8 ||(Cur_FocusParam >= 16 && Cur_FocusParam <= 23))
  {
      //在电机运行时设置这几项参数 报错
      if(Motor.IsWorking)
        return 1;
  }
  return 0;
}

/******************************************************************************
*函数名： menu_2_Enter(uint8 Key_Mode)
*作用：   在参数界面下Enter键调用函数
*******************************************************************************/
void menu_2_Enter(uint8 Key_Mode)
{
	MUME_LIST *List = &Menu_Param.stPARAM_List;  

	if(Key_Mode == KEY_UP || Key_Mode == KEY_LUP)
	{
        if(menu_ChParam())
        {
            Cur_FocusMenu = 3;
            Dis_SetString(" Err",0);
            Msg_Mark = 1;
            return;
        } 
        
        if(Menu_Param.stList[Cur_FocusParam].Val == Temp_Param)
        {
            Cur_FocusMenu   = 3;
            Msg_Mark = 1;
            Dis_SetString(" End",0);
            return;
        }            
        
        Menu_Param.stList[Cur_FocusParam].Val = Temp_Param;
        EEPROM_ChMark();
	/*---------------------------------------------------------------------------------------------*/      
		  //系统初始化按钮被按下时
		if(Cur_FocusParam == 23 && List->ReSet.Val == 2)
		{
			MenuData_Init();                      		//菜单参数初始化
			MotorParam_LockCtrl((BOOL)List->Err_Report.Val);
		}
		  //其他参数项被修改时
		else
		{
			//堵转使能设定
			if(Cur_FocusParam == 15)
				MotorParam_LockCtrl((BOOL)List->Err_Report.Val);
			else if(Cur_FocusParam == 7 || Cur_FocusParam == 8)
			{
				if(List->Cur_Speed.Val > List->Max_Speed.Val)
				  List->Cur_Speed.Val = List->Max_Speed.Val;
				else if(List->Cur_Speed.Val < List->Min_Speed.Val)
				  List->Cur_Speed.Val = List->Min_Speed.Val;
				
				if(List->Rev_CorSpd.Val > List->Max_Speed.Val)
				  List->Rev_CorSpd.Val = List->Max_Speed.Val;
				else if(List->Rev_CorSpd.Val < List->Min_Speed.Val)
				  List->Rev_CorSpd.Val = List->Min_Speed.Val;
				
				if(List->Rev_BakSpd.Val > List->Max_Speed.Val)
				  List->Rev_BakSpd.Val = List->Max_Speed.Val;
                else if(List->Rev_BakSpd.Val < List->Min_Speed.Val)
				  List->Rev_BakSpd.Val = List->Min_Speed.Val;
				
				Menu_GetRadioSpdVal();
			}
			else if(Cur_FocusParam == 16)
				ucCys_Mark = 0;			
		}
		  
		if(((Cur_FocusParam == 1) || (Cur_FocusParam == 2)) && List->Dis_Mode.Val == 2)
			Menu_GetRadioSpdVal();
			
		Cur_FocusMenu   = 3;
		Msg_Mark = 1;
		Dis_SetString(" End",0);
	}
}

/******************************************************************************
*函数名： static int16 menu_SpeedParam(int16 Spd)
*作用：   在设置最大最小速度时的保护处理 避免出现 最高速度小于最低速度的情况
*******************************************************************************/
static int16 menu_SpeedParam(int16 Spd)
{
    //处理当最高转速小于最低转速时的情况
    if(Cur_FocusParam == 7)
    {
      if(Spd < Menu_Param.stPARAM_List.Min_Speed.Val)
        Spd = Menu_Param.stPARAM_List.Min_Speed.Val;
    }
    else if(Cur_FocusParam == 8)
    {
      if(Spd > Menu_Param.stPARAM_List.Max_Speed.Val)
        Spd = Menu_Param.stPARAM_List.Max_Speed.Val;
    }
    
    return Spd;
}

/******************************************************************************
*函数名： menu_2_Up(uint8 Key_Mode)
*作用：   在参数界面下Up键调用函数
*******************************************************************************/
void menu_2_Up(uint8 Key_Mode)
{
    Temp_Param = Menu_ParaUp(Menu_Param.stList[Cur_FocusParam].Range->Max,Temp_Param,Key_Mode);
    Temp_Param = menu_SpeedParam(Temp_Param);
	
	if(Cur_FocusParam == 2 || Cur_FocusParam == 9 || Cur_FocusParam == 11 || Cur_FocusParam == 12 || Cur_FocusParam == 14)
    	Dis_SetNum(Temp_Param,3);
    else 
	    Dis_SetNum(Temp_Param,0);
}
 
/******************************************************************************
*函数名： menu_2_Down(uint8 Key_Mode)
*作用：   在参数界面下Down键调用函数
*******************************************************************************/
void menu_2_Down(uint8 Key_Mode)
{
    Temp_Param = Menu_ParaDown(Menu_Param.stList[Cur_FocusParam].Range->Min,Temp_Param,Key_Mode);
    Temp_Param = menu_SpeedParam(Temp_Param);
  	
	if(Cur_FocusParam == 2 || Cur_FocusParam == 9 || Cur_FocusParam == 11 || Cur_FocusParam == 12 || Cur_FocusParam == 14)
    	Dis_SetNum(Temp_Param,3);
    else 
	    Dis_SetNum(Temp_Param,0);
}

/******************************************************************************
*函数名： void menu_E_Enter(uint8 Key_Mode)
*作用：   报错界面下的处理 退出后可照常运行
*******************************************************************************/
void menu_E_Enter(uint8 Key_Mode)
{
  if(Key_Mode == KEY_DOWN)
  {
      Cur_FocusMenu  = 0;
      Cur_FocusParam = 1;
      Dis_SetNum(MotorParam_GetDisSpd(),0);
	  Motor.Status = MIdle;
  }
}

/*---------------------------------------------------------------------------*/
/*************************************************************
* 函数名称：void Menu_HintWinCtrl(void)
* 功能介绍：提示界面的处理
*************************************************************/
void Menu_HintWinCtrl(void)
{
	if(Msg_Mark) //提示界面处理
	{
		if(Msg_Delay++ > 700) //超过 1.4S
		{
			Msg_Mark = 0;
		  	Msg_Delay = 0;
		  	ID_ReFresh();
		}
	}
}


/*************************************************************
* 函数名称：void MenuCtrl_Process(void)
* 功能介绍：菜单控制进程
*************************************************************/
void MenuCtrl_Process(void)
{
	uint16 Key_Type = 0; //记录哪个按键被按下
	uint16 Key_Mode = 0; //按下 松开等模式判断
	  
	Menu_HintWinCtrl();
	  
	if(Menu_Signal == NoKey) return;
	  
	Key_Type = Menu_Signal & 0x0f;
	Key_Mode = Menu_Signal & 0xf0;
	Menu_Signal = NoKey;
	  
	switch(Key_Type)
	{
		case K_Mode:
			SysMenu_Menu[Cur_FocusMenu].onModeFun(Key_Mode);
			break;
		case K_Enter:
			SysMenu_Menu[Cur_FocusMenu].onEnterFun(Key_Mode);
			break;
		case K_Up:			
			SysMenu_Menu[Cur_FocusMenu].onUpFun(Key_Mode);			
		 	break;
		case K_Down:		
			SysMenu_Menu[Cur_FocusMenu].onDownFun(Key_Mode);			
			break;
		default:
			break;
	}    
}

