#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "type.h"

//EEPROM的保存起始地址
#define EE_START 0x4000
 
//EEPROM的保存标志位
#define EE_MARK 0x4100



//EEPROM的保存标志位的判别值
#define EE_MARK_VAL 	0x17

#define SAVE_ALL        0xff
#define SAVE_MOTOR      0xfe
#define SAVE_NONE		0xfd

typedef struct
{
	uint8 Mark;
	uint8 Enable;
	uint16 Delay;
}DLY_SAVE;

#define DLY_SAVE_DEFAULT {0,0,0}

extern uint8 EEPROM_Count;


extern void EEPROM_Process(void);
extern void EERPROM_Init(void);
extern void EEPROM_ChMark(void);

#endif
