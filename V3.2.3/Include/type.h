/***************************************************************************** 
* Copyright (c) 2013,  WenZhou ESON  All rights reserved. 
* �ļ����ƣ� type.h 
* �ĵ���ʷ 
* �汾��	�޶�ʱ��	�޸���   
V1.0.0  	2013-5-4	������
1������
*****************************************************************************/
#ifndef __TYPE_H__
#define __TYPE_H__


/******************************************************************
						    �궨��
******************************************************************/	
#ifndef TRUE
#define TRUE      1
#endif

#ifndef FALSE   
#define FALSE     0
#endif

#define _IQsat(a,Max,Min) (((a) > (Max)) ? (Max) : (((a) < (Min)) ? (Min) : (a)))
/******************************************************************
						  ������������
******************************************************************/
typedef unsigned char  	uint8;		/* defined for unsigned 8-bits integer variable 	�޷���8λ���ͱ���  */
typedef signed   char  	int8;		/* defined for signed 8-bits integer variable		�з���8λ���ͱ���  */
typedef unsigned int 		uint16;		/* defined for unsigned 16-bits integer variable 	�޷���16λ���ͱ��� */
typedef signed   int 		int16;		/* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
typedef unsigned long   	uint32;		/* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
typedef signed   long   	int32;		/* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */
typedef float          	fp32;		/* single precision floating point variable  		�����ȸ�������32λ���ȣ�*/

typedef union
{
	uint16 HalfWord;
	struct
	{
		uint8 HByte;
		uint8 LByte;
	}BYTE;
}HALF_WORD;





#endif
/*****************************************************************************************************
*                                      End Of File													 *
******************************************************************************************************/

