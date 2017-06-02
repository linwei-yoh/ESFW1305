#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "type.h"

extern void Dis_SetNum(uint16 Val,uint8 Point);
extern void Dis_SetString(uint8 *cData,uint8 Point);

extern void Display_Init(void);
extern void Display_Process(void);

extern void Display_FdbSetSpd();

#endif
