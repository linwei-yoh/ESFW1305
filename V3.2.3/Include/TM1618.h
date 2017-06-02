#ifndef __TM1618_H__
#define __TM1618_H__

#include "iostm8s103f3.h"
#include "type.h"

#define TM_CLK		PB_ODR_ODR4
#define TM_STB		PA_ODR_ODR3

#define DIO_W  	  	PB_ODR_ODR5
#define DIO_R      	PB_IDR_IDR5


typedef struct
{
	uint8 KS1:1;
	uint8 KS2:1;
	uint8 KS3:1;
	uint8 KS4:1;
	uint8 KS5:1;
	uint8 Rev:3;
}KEY_VAL;


extern void TM_Init(void);
extern void TM_Command(uint8 ucCom);
extern void TM_SendByte(uint8 ucData);

extern void TM_KeyRead(void);
extern KEY_VAL TM_KeyVal;


#endif
