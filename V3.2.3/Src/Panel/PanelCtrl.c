#include "PanelCtrl.h"
#include "MenuData.h"
#include "KeyCtrl.h"


/***********************************************
** 函数名称：void PanelCtrl_Init(void)
** 功能描述：面板控制初始化
***********************************************/
void PanelCtrl_Init(void)
{
    TM_Init();
    Display_Init();
    KeyCtrl_Init();
}

/***********************************************
** 函数名称：void PanelCtrl_Process(void)
** 功能描述：面板控制进程
***********************************************/
void PanelCtrl_Process(void)
{
    KeyCtrl_Process(); //获取按键信号
    Display_Process(); //显示处理
}