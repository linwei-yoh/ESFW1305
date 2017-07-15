#ifndef __KEYCTRL_H__
#define __KEYCTRL_H__

#include "type.h"

#define KEY_ENTER  PA_IDR_IDR1 
#define KEY_MODE   PA_IDR_IDR2
#define FWD_IN     PC_IDR_IDR7
#define REV_IN     PD_IDR_IDR2 

//*------------------------------数据类型定义----------------------------------*
#define NoKey 		        0x00

#define KEY_UP 		        0xA0	  
#define KEY_DOWN 	        0xB0
#define KEY_LUP 	        0xC0            //长按松开
#define KEY_LDOWN         	0xD0	        //长按按下

#define K_Stop 		        0x01
#define K_Rev 		        0x02        
#define K_Fwd 		        0x03   

#define K_Up               	0x04     
#define K_Down            	0x05        
#define K_Mode 		        0x06
#define K_Enter 	        0x07

#define K_MENU				0x04
 

extern uint8 Menu_Signal;
extern uint8 Motor_Signal;

extern void KeyCtrl_Init(void);
extern void KeyCtrl_Process(void);
#endif
