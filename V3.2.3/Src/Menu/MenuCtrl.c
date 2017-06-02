#include "MenuCtrl.h"
#include "MenuData.h"
#include "PanelCtrl.h"
#include "Eeprom.h"
#include "MotorCtrl.h"
#include "MotorParam.h"
#include "Display.h"
#include "MotorModeCtrl.h"

//��ǰ�˵�ID 0:�����˵�  1: ����ѡ��˵� 2:���������˵� 3:��ʾ���� 4:���ϱ�������
int8 Cur_FocusMenu  = 0;  
//�����б�   ��ֵ��Ӧ ����ѡ����  �����б��1 ��ʼ ��(LIST_Size - 1)���� 
int8 Cur_FocusParam = 1;  

//���������������ʱ �Ƚ��ò��������ֵ ���浽�ñ�����
static int16 Temp_Param   = 0;  
//������ʾ�����־λ
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
                          {menu_0_Mode,menu_Disable,menu_0_Up,menu_0_Down}, 		//�ٶ���ʾ����
                          {menu_1_Mode,menu_1_Enter,menu_1_Up,menu_1_Down},  		//������ѡ�����
                          {menu_2_Mode,menu_2_Enter,menu_2_Up,menu_2_Down},  		//�����趨����
                          {menu_Disable,menu_Disable,menu_Disable,menu_Disable},   	//����������ʾ����
                          {menu_Disable,menu_E_Enter,menu_Disable,menu_Disable}     //�������
                        };


/*------------------------------�˵���������---------------------------------*/

/******************************************************************************
*�������� 	void Menu_GetRadioSpdVal(void)
*����˵��	���л���������ʾģʽʱ ������ǰ�趨�ٶ�
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
*�������� uint16 Menu_SpdUpWithRio(uint8 Key_Mode)
*������	  Key_Mode ����״̬
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
*�������� uint16 Menu_SpdDownWithRio(uint8 Key_Mode)
*������	  Key_Mode ����״̬
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
*�������� Menu_ParaUp
*������	  Max ���Χ Val ��ǰֵ Key_Mode ����״̬
*���ã�   ������������ӵ��õĺ���
*******************************************************************************/
uint16 Menu_ParaUp(int16 Max,int16 Val,uint8 Key_Mode)
{
    if(Key_Mode == KEY_UP)  //���������ɿ�
        Val++;
    else if(Key_Mode == KEY_LDOWN) //������������
        Val += LONG_KEY_Val;
    
    if(Val > Max)
        Val = Max;
     
    return Val;
}

/******************************************************************************
*�������� Menu_ParaDown
*������	  Min ��С��Χ Val ��ǰֵ Key_Mode ����״̬
*���ã�   ������������ٵ��õĺ���
*******************************************************************************/
uint16 Menu_ParaDown(int16 Min,int16 Val,uint8 Key_Mode)
{
    if(Key_Mode == KEY_UP)  //���������ɿ�
        Val--;
    else if(Key_Mode == KEY_LDOWN) //������������
        Val -= LONG_KEY_Val; 
    
    if(Val < Min)
        Val = Min;
    
    return Val;
}

/*************************************************************
* �������ƣ�void menu_Disable(uint8 Key_Mode) {}
* ���ܽ��ܣ��ղ���
*************************************************************/
void menu_Disable(uint8 Key_Mode) {}


/******************************************************************************
*�������� menu_0_Mode(uint8 Key_Mode)
*���ã�   ���ٶ���ʾ������Mode�����ú���
*******************************************************************************/
void menu_0_Mode(uint8 Key_Mode)
{
  if(Key_Mode == KEY_LDOWN)
  {
    Cur_FocusMenu  = 1;  //�л�������ѡ��˵�
    Cur_FocusParam = 1;  //����ѡ����
    Dis_SetString("F-01",0);
  }	
}

/******************************************************************************
*�������� menu_0_Up(uint8 Key_Mode)
*���ã�   ���ٶȿ��ƽ�����Up�����ú���
*******************************************************************************/
void menu_0_Up(uint8 Key_Mode)
{
  	uint16 Dis_Mode = Menu_Param.stPARAM_List.Dis_Mode.Val;
	uint16 NewSpd = 0;
	
	if(Key_Mode != KEY_UP && Key_Mode !=  KEY_LDOWN) return;
	if(Menu_Param.stPARAM_List.Rev_En.Val == 2) return; //ѭ��ģʽ���� ������
	
	//�����ð�������
	if(Menu_Param.stPARAM_List.SpeedCtrl_Mode.Val == 1)  
  	{
	  	NewSpd = Menu_Param.stPARAM_List.Cur_Speed.Val;
		//������ʾģʽ
		if(Dis_Mode == 1)
		{
			NewSpd = Menu_ParaUp(Menu_Param.stPARAM_List.Max_Speed.Val,NewSpd,Key_Mode);
			Menu_Param.stPARAM_List.Cur_Speed.Val = NewSpd;
			Dis_SetNum(NewSpd,0);			
		}
		//������ʾģʽ
		else if(Dis_Mode == 2)
			Dis_SetNum(Menu_SpdUpWithRio(Key_Mode),0);
		//������ʾģʽ
		else if(Dis_Mode == 3)
		{
		  	NewSpd = Menu_ParaUp(Menu_Param.stPARAM_List.Max_Speed.Val,NewSpd,Key_Mode);
			Menu_Param.stPARAM_List.Cur_Speed.Val = NewSpd;
			Display_FdbSetSpd(); //�趨�ٶ���ʾ
			Dis_SetNum(NewSpd,0);	
		}
		EEPROM_ChMark();
  	}	
}

/******************************************************************************
*�������� menu_0_Down(uint8 Key_Mode)
*���ã�   ���ٶȿ��ƽ�����Mode�����ú���
*******************************************************************************/
void menu_0_Down(uint8 Key_Mode)
{
  	uint16 Dis_Mode = Menu_Param.stPARAM_List.Dis_Mode.Val;
	uint16 NewSpd = 0;
	
	if(Key_Mode != KEY_UP && Key_Mode !=  KEY_LDOWN) return;
	if(Menu_Param.stPARAM_List.Rev_En.Val == 2) return; //ѭ��ģʽ���� ������
	
	//�����ð�������
	if(Menu_Param.stPARAM_List.SpeedCtrl_Mode.Val == 1)  
  	{
	  	NewSpd = Menu_Param.stPARAM_List.Cur_Speed.Val;
		//������ʾģʽ
		if(Dis_Mode == 1)
		{
			NewSpd = Menu_ParaDown(Menu_Param.stPARAM_List.Min_Speed.Val,NewSpd,Key_Mode);
			Menu_Param.stPARAM_List.Cur_Speed.Val = NewSpd;
			Dis_SetNum(NewSpd,0);			
		}
		//������ʾģʽ
		else if(Dis_Mode == 2)
			Dis_SetNum(Menu_SpdDownWithRio(Key_Mode),0);
		//������ʾģʽ
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
*�������� menu_1_Mode(uint8 Key_Mode)
*���ã�   �ڹ���ѡ�������Mode�����ú���
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
*�������� menu_1_Enter(uint8 Key_Mode)
*���ã�   �ڹ���ѡ�������Enter�����ú���
*******************************************************************************/
void menu_1_Enter(uint8 Key_Mode)
{
  	MUME_LIST *List = &Menu_Param.stPARAM_List;  
	
  	if(Key_Mode == KEY_UP || Key_Mode == KEY_LUP)
  	{
    	Cur_FocusMenu = 2;
		Temp_Param = Menu_Param.stList[Cur_FocusParam].Val;            //����ԭ����  
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
*�������� ID_ReFresh(void)
*���ã�   ˢ�¹���ѡ��˵����
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
*�������� menu_1_Up(uint8 Key_Mode)
*���ã�   �ڹ���ѡ�������UP�����ú���
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
*�������� menu_1_Down(uint8 Key_Mode)
*���ã�   �ڹ���ѡ�������Down�����ú���
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
*�������� menu2OnMode(uint8 Key_Mode)
*���ã�   �ڲ���������Mode�����ú���
*******************************************************************************/
void menu_2_Mode(uint8 Key_Mode)
{
  if(Key_Mode == KEY_UP || Key_Mode == KEY_LUP)
    ID_ReFresh();
}

/******************************************************************************
*�������� static uint16 menu_ChParam(void)
*���ã�   �����޸ļ��
*******************************************************************************/
static uint16 menu_ChParam(void)
{
  //������״̬�� �������޸ĵĲ����� ����ȷ�� ���趨������ʾ
  if(Cur_FocusParam == 4 ||Cur_FocusParam == 5 || Cur_FocusParam == 7 
      ||Cur_FocusParam == 8 ||(Cur_FocusParam >= 16 && Cur_FocusParam <= 23))
  {
      //�ڵ������ʱ�����⼸����� ����
      if(Motor.IsWorking)
        return 1;
  }
  return 0;
}

/******************************************************************************
*�������� menu_2_Enter(uint8 Key_Mode)
*���ã�   �ڲ���������Enter�����ú���
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
		  //ϵͳ��ʼ����ť������ʱ
		if(Cur_FocusParam == 23 && List->ReSet.Val == 2)
		{
			MenuData_Init();                      		//�˵�������ʼ��
			MotorParam_LockCtrl((BOOL)List->Err_Report.Val);
		}
		  //����������޸�ʱ
		else
		{
			//��תʹ���趨
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
*�������� static int16 menu_SpeedParam(int16 Spd)
*���ã�   �����������С�ٶ�ʱ�ı������� ������� ����ٶ�С������ٶȵ����
*******************************************************************************/
static int16 menu_SpeedParam(int16 Spd)
{
    //�������ת��С�����ת��ʱ�����
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
*�������� menu_2_Up(uint8 Key_Mode)
*���ã�   �ڲ���������Up�����ú���
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
*�������� menu_2_Down(uint8 Key_Mode)
*���ã�   �ڲ���������Down�����ú���
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
*�������� void menu_E_Enter(uint8 Key_Mode)
*���ã�   ��������µĴ��� �˳�����ճ�����
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
* �������ƣ�void Menu_HintWinCtrl(void)
* ���ܽ��ܣ���ʾ����Ĵ���
*************************************************************/
void Menu_HintWinCtrl(void)
{
	if(Msg_Mark) //��ʾ���洦��
	{
		if(Msg_Delay++ > 700) //���� 1.4S
		{
			Msg_Mark = 0;
		  	Msg_Delay = 0;
		  	ID_ReFresh();
		}
	}
}


/*************************************************************
* �������ƣ�void MenuCtrl_Process(void)
* ���ܽ��ܣ��˵����ƽ���
*************************************************************/
void MenuCtrl_Process(void)
{
	uint16 Key_Type = 0; //��¼�ĸ�����������
	uint16 Key_Mode = 0; //���� �ɿ���ģʽ�ж�
	  
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

