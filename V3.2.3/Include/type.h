/***************************************************************************** 
* Copyright (c) 2013,  WenZhou ESON  All rights reserved. 
* 文件名称： type.h 
* 文档历史 
* 版本号	修订时间	修改者   
V1.0.0  	2013-5-4	余泽民
1、初稿
*****************************************************************************/
#ifndef __TYPE_H__
#define __TYPE_H__


/******************************************************************
						    宏定义
******************************************************************/	
#ifndef TRUE
#define TRUE      1
#endif

#ifndef FALSE   
#define FALSE     0
#endif

#define _IQsat(a,Max,Min) (((a) > (Max)) ? (Max) : (((a) < (Min)) ? (Min) : (a)))
/******************************************************************
						  数据类型申明
******************************************************************/
typedef unsigned char  	uint8;		/* defined for unsigned 8-bits integer variable 	无符号8位整型变量  */
typedef signed   char  	int8;		/* defined for signed 8-bits integer variable		有符号8位整型变量  */
typedef unsigned int 		uint16;		/* defined for unsigned 16-bits integer variable 	无符号16位整型变量 */
typedef signed   int 		int16;		/* defined for signed 16-bits integer variable 		有符号16位整型变量 */
typedef unsigned long   	uint32;		/* defined for unsigned 32-bits integer variable 	无符号32位整型变量 */
typedef signed   long   	int32;		/* defined for signed 32-bits integer variable 		有符号32位整型变量 */
typedef float          	fp32;		/* single precision floating point variable  		单精度浮点数（32位长度）*/

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

