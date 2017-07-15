#include "MenuData.h"
#include "SpdCalc.h"


V_RANGE CH2={1,2},CH3={1,3},CH4={1,4},CHR={1,100},CHM={10,9999},CHMax={500,3000},CHMin={90,1000},KMax={0,9999},RMax={1,9999},FdbR={2,32};
V_RANGE SpR={90,3000};
#define MENU_PARAM_DEFAULT 	{{{120,&CHMin},{1,&CH3},{10,&CHM},{1,&CH4},		\
							  {1,&CH3},{1,&CH2},{2,&CH2},{1400,&CHMax},		\
							  {120,&CHMin},{10,&CHR},{1,&CH2},{1,&CHR},		\
							  {10,&CHR},{1,&CH2},{1,&CHR},{1,&CH2},			\
							  {1,&CH2},{2,&RMax},{5,&KMax},{5,&KMax},		\
                        	  {5,&RMax},{1000,&SpR},{1000,&SpR},{1,&CH2},	\
							  {2,&FdbR}}}

//菜单源数据
MENU_PARAM Menu_Param = MENU_PARAM_DEFAULT;

int16 Radio_Spd = 0;
/******************************************************************************
*函数名：void MenuData_Init(void)
*作用：  菜单参数恢复初始状态
*******************************************************************************/
void MenuData_Init(void)
{
    Menu_Param.stPARAM_List.Cur_Speed.Val       = 120;
    Menu_Param.stPARAM_List.Dis_Mode.Val        = 1;
    Menu_Param.stPARAM_List.Ratio.Val           = 10;
    Menu_Param.stPARAM_List.Control_Mode.Val    = 1;
    Menu_Param.stPARAM_List.Rotate_Mode.Val     = 1;
    Menu_Param.stPARAM_List.Rotate_Dir.Val      = 1;
    Menu_Param.stPARAM_List.SpeedCtrl_Mode.Val  = 2;
    Menu_Param.stPARAM_List.Max_Speed.Val       = 1400;
    Menu_Param.stPARAM_List.Min_Speed.Val       = 120;
    Menu_Param.stPARAM_List.Cor_BTime.Val       = 10;
    Menu_Param.stPARAM_List.Cor_Mode.Val        = 1;
    Menu_Param.stPARAM_List.Cor_ETime.Val       = 1; 
    Menu_Param.stPARAM_List.Bak_BTime.Val       = 10;
    Menu_Param.stPARAM_List.Bak_Mode.Val        = 1;
    Menu_Param.stPARAM_List.Bak_ETime.Val       = 1;
    Menu_Param.stPARAM_List.Err_Report.Val      = 1;
    Menu_Param.stPARAM_List.Rev_En.Val          = 1;
    Menu_Param.stPARAM_List.Rev_Num.Val         = 1;
    Menu_Param.stPARAM_List.Rev_FRTime.Val      = 5;
    Menu_Param.stPARAM_List.Rev_RRTime.Val      = 5;
    Menu_Param.stPARAM_List.Rev_STime.Val       = 5;
    Menu_Param.stPARAM_List.Rev_CorSpd.Val      = 1000;
    Menu_Param.stPARAM_List.Rev_BakSpd.Val      = 1000;
	Menu_Param.stPARAM_List.ReSet.Val           = 1;
    Menu_Param.stPARAM_List.SpdFdb.Val          = 2;      
}




