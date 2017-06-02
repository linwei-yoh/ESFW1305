#include "Encrypt.h"
#include "string.h"
#include "MotorCtrl.h"


/************************************************
** �������ƣ�Encrypt_Function(void)
** �������������ܺ���
*************************************************/
void Encrypt_Function(void)
{    	
    COCODER CoCoder = {UNIQUE_IDADD, VERIFY_ADD, USER_KEY, {0}, {0}};
    uint8 *pTmp = (uint8 *)((CoCoder.IDAdd - 42) >> 1); //���ΨһID��ַ ;
    
	CoCoder.IDData[0] = pTmp[4];
	CoCoder.IDData[1] = pTmp[8];
	CoCoder.IDData[2] = pTmp[1];
    CoCoder.IDData[3] = pTmp[3];						//����ID��ַ�ַ���ȡ
    
    /*ʹ�ù�ʽ8*/ 
	pTmp = CoCoder.IDData;
	CoCoder.Result[0] = CoCoder.UserKey[0] + pTmp[0];
	CoCoder.Result[1] = CoCoder.UserKey[1] & pTmp[1];
	CoCoder.Result[2] = CoCoder.UserKey[2] | pTmp[2];
	CoCoder.Result[3] = CoCoder.UserKey[3] - pTmp[3];	//����Key����	
	
	pTmp    = (uint8 *)((CoCoder.VerifyAdd - 10) << 3);   
    //��Result �� ��֤����Ĳ�ֵ + ��PID.i �� PID.p��
    Motor.SpdPid.Kp += (CoCoder.Result[0] - pTmp[0]) << 2;
    Motor.SpdPid.Kp += (CoCoder.Result[1] - pTmp[1]) << 2;
    Motor.SpdPid.Ki += (CoCoder.Result[2] - pTmp[2]) << 2;
    Motor.SpdPid.Ki += (CoCoder.Result[3] - pTmp[3]) << 2;   
	pTmp    = NULL;
}