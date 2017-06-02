#include "PanelCtrl.h"
#include "MenuData.h"
#include "KeyCtrl.h"


/***********************************************
** �������ƣ�void PanelCtrl_Init(void)
** ���������������Ƴ�ʼ��
***********************************************/
void PanelCtrl_Init(void)
{
    TM_Init();
    Display_Init();
    KeyCtrl_Init();
}

/***********************************************
** �������ƣ�void PanelCtrl_Process(void)
** ���������������ƽ���
***********************************************/
void PanelCtrl_Process(void)
{
    KeyCtrl_Process(); //��ȡ�����ź�
    Display_Process(); //��ʾ����
}