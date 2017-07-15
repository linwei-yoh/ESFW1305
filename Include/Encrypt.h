#ifndef __ENCRYPT_H__
#define __ENCRYPT_H__

#include "type.h"

#define 	STM8S103
/*****************************************************************************
						         宏定义
*****************************************************************************/
#ifdef STM8S103
     #define     UID_ADD            (0X4865UL)
     #define     UNIQUE_IDADD		((0x4865UL << 1) + 42)	
#endif

#ifdef STM8S105
     #define     UID_ADD            (0x48CD)
     #define     UNIQUE_IDADD		((0x48CD << 1) + 42)
#endif

#define VERIFY_ADD				((0x9F50 >> 3) + 10)	 //验证码地址 待修改		
#define USER_KEY				{0x5F,0xC4,0x7A,0x06}

/*****************************************************************************
						       数据类型定义
*****************************************************************************/
typedef struct
{
	uint16  IDAdd;				//唯一ID号地址
	uint16  VerifyAdd;			//验证码地址
	uint8   UserKey[4];			//用户密钥
	uint8   IDData[4];			//排序后的ID号
	uint8   Result[4];			//解密算法计算结果	
}COCODER;

/*****************************************************************************
						       函数接口申明
*****************************************************************************/
extern void Encrypt_Function(void);
#endif
