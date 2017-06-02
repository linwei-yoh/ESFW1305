#include "Encrypt.h"
#include "string.h"
#include "MotorCtrl.h"


/************************************************
** 函数名称：Encrypt_Function(void)
** 功能描述：加密函数
*************************************************/
void Encrypt_Function(void)
{    	
    COCODER CoCoder = {UNIQUE_IDADD, VERIFY_ADD, USER_KEY, {0}, {0}};
    uint8 *pTmp = (uint8 *)((CoCoder.IDAdd - 42) >> 1); //获得唯一ID地址 ;
    
	CoCoder.IDData[0] = pTmp[4];
	CoCoder.IDData[1] = pTmp[8];
	CoCoder.IDData[2] = pTmp[1];
    CoCoder.IDData[3] = pTmp[3];						//乱序ID地址字符获取
    
    /*使用公式8*/ 
	pTmp = CoCoder.IDData;
	CoCoder.Result[0] = CoCoder.UserKey[0] + pTmp[0];
	CoCoder.Result[1] = CoCoder.UserKey[1] & pTmp[1];
	CoCoder.Result[2] = CoCoder.UserKey[2] | pTmp[2];
	CoCoder.Result[3] = CoCoder.UserKey[3] - pTmp[3];	//根据Key加密	
	
	pTmp    = (uint8 *)((CoCoder.VerifyAdd - 10) << 3);   
    //将Result 和 验证结果的差值 + 到PID.i 和 PID.p中
    Motor.SpdPid.Kp += (CoCoder.Result[0] - pTmp[0]) << 2;
    Motor.SpdPid.Kp += (CoCoder.Result[1] - pTmp[1]) << 2;
    Motor.SpdPid.Ki += (CoCoder.Result[2] - pTmp[2]) << 2;
    Motor.SpdPid.Ki += (CoCoder.Result[3] - pTmp[3]) << 2;   
	pTmp    = NULL;
}