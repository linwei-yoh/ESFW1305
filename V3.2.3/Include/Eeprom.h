#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "type.h"

//EEPROM�ı�����ʼ��ַ
#define EE_START 0x4000
 
//EEPROM�ı����־λ
#define EE_MARK 0x4100



//EEPROM�ı����־λ���б�ֵ
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
