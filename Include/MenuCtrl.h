#ifndef __MENUCTRL_H__
#define __MENUCTRL_H__


#include "type.h"

typedef struct
{
	void (*onModeFun)(uint8 Key_Mode);
	void (*onEnterFun)(uint8 Key_Mode);
	void (*onUpFun)(uint8 Key_Mode);
	void (*onDownFun)(uint8 Key_Mode);
}SYSMENU;

extern	int8 Cur_FocusMenu;
extern void MenuCtrl_Process(void);
extern void Menu_GetRadioSpdVal(void);

extern uint8 ValChMark;
extern uint8 CycChMark;

#endif
