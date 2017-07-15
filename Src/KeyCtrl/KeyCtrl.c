#include "iostm8s103f3.h"
#include "TM1618.h"
#include "MenuData.h"
#include "MotorCtrl.h"

#include "KeyCtrl.h"

static uint8 IO_InRev = 1;
static uint8 IO_InFwd = 1;  // 0 ���Ǵ����ź�

uint8 Menu_Signal = NoKey;
uint8 Motor_Signal = NoKey;
/*****************************************************************************
** �������ƣ�uint8 Key_Read(void)
** ���������������źŶ�ȡ
*****************************************************************************/
static uint8 Key_Read(void)
{
	uint8 Read_Val = NoKey;
  
	TM_KeyRead();
	
	if(TM_KeyVal.KS4)
	  Read_Val = K_Stop;
	else if(TM_KeyVal.KS5)
	  Read_Val = K_Rev;
	else if(TM_KeyVal.KS3)
	  Read_Val = K_Fwd;
	else if(TM_KeyVal.KS2)
	  Read_Val = K_Down;
	else if(TM_KeyVal.KS1)
	  Read_Val = K_Up;	  
	else if(KEY_ENTER == 0)
	  Read_Val = K_Enter;
	else if(KEY_MODE == 0)
	  Read_Val = K_Mode;
	
	return Read_Val;
}

/*****************************************************************************
** �������ƣ�uint8 KeyCtrl_Scan(void)
** ��������������ɨ�� ��ȡ�����ź�
*****************************************************************************/
static uint8 KeyCtrl_Scan(void)
{
 	static uint8 Scan_Sta 	= 0;
	static uint8 Key_Prev	= NoKey; //��һ�ΰ���
	static uint16 Key_Dly  = 0;
	uint8 Key_Val 		 	= NoKey;
    
	Key_Val = Key_Read();
	switch(Scan_Sta)
	{
      case 0: //������ʼɨ��
        if(Key_Val != NoKey)
        {
            Key_Prev = Key_Val;
            Key_Dly = 0;
            Scan_Sta = 1;
        }
        break;
      case 1://ȥ���ж�
        if(Key_Val == NoKey || Key_Val != Key_Prev)
            Scan_Sta = 0;
        else if(Key_Dly++ >= 5)
        {
            Key_Dly = 0;
            Scan_Sta = 2;
            return (KEY_DOWN | Key_Prev);   //������Ч��������
        }            
        break;
      case 2://���������ж�
        if(Key_Val == NoKey || Key_Val != Key_Prev)//�ް����� ����ֵ�ı�
        {
            Scan_Sta = 0;
            return (KEY_UP|Key_Prev);           //������Ч�����ɿ�
        }
        else if(Key_Dly++ > 500) //����1S
        {
            Key_Dly = 0;
            Scan_Sta = 3;
        }
        break;
      case 3://�����ſ��ж�
        if(Key_Val == NoKey || Key_Val != Key_Prev)//�ް����� ����ֵ�ı�
        {
            Scan_Sta = 0;
            return (KEY_LUP | Key_Prev);//������Ч�����������ɿ�
        }
        else if(Key_Dly++ > 50) //ÿ100ms����һ�γ����ź�
        {
            Key_Dly = 0;
            return (KEY_LDOWN | Key_Prev);	//���ذ���������Ϣ 
        }
        break;
      default:
        break;
	}
	
  	return NoKey;
}

/*************************************************************
* �������ƣ� 	IO_InPutCheck(void)
* ���ܽ��ܣ�	��������ź������ȥ������
*************************************************************/
void IO_InPutCheck(void)
{
	  static uint8 Tmp_InRev = 0xff;
	  static uint8 Tmp_InFwd = 0xff;
	  
	  Tmp_InRev = (Tmp_InRev << 1) | REV_IN;
	  Tmp_InFwd = (Tmp_InFwd << 1) | FWD_IN;
	  
	  if(Tmp_InRev == 0)
		IO_InRev = 0;
	  else if(Tmp_InRev == 0xff)
		IO_InRev = 1;
	  
	  if(Tmp_InFwd == 0)
		IO_InFwd = 0;
	  else if(Tmp_InFwd == 0xff)
		IO_InFwd = 1;
} 

/*************************************************************
* �������ƣ�uint8 IO_InPutNoStop(void)
* ���ܽ��ܣ����stop��Чģʽ�� ��ӿ��ص��źŻ�ȡ
*************************************************************/
uint8 IO_InPutNoStop(void)
{
  if(IO_InFwd == 0 && IO_InRev == 1)
    return K_Fwd;
  else if(IO_InFwd == 1 && IO_InRev == 0)
    return K_Rev;
  else if(IO_InFwd == 1 && IO_InRev == 1)
    return K_Stop;
  
  return NoKey;
}

/*************************************************************
* �������ƣ�uint8 IO_InPutWithStop(void)
* ���ܽ��ܣ����stop��Чģʽ�� ��Ӱ�ť���źŻ�ȡ
*************************************************************/
uint8 IO_InPutWithStop(void)
{
  if(IO_InFwd == 0 && IO_InRev == 1)
    return K_Fwd;
  else if(IO_InFwd == 1 && IO_InRev == 0)
    return K_Rev;
  
  return NoKey;
}

/*****************************************************
** �������ƣ�void KeyCtrl_Init (void)
** �����������������Ƴ�ʼ��
******************************************************/
void KeyCtrl_Init(void)
{
  	/*ENTER*/
	PA_DDR_DDR1 = 0;		//����PA1Ϊ����
	PA_CR1_C11  = 1;		//��������
	PA_CR2_C21  = 0;
	
	/*MODE*/
	PA_DDR_DDR2 = 0;		//����PA2Ϊ����
	PA_CR1_C12  = 1;		//��������
	PA_CR2_C22  = 0;	
    
    /*FWDI*/
	PC_DDR_DDR7 = 0;		//����PC7����
	PC_CR1_C17  = 0;		//��������
	PC_CR2_C27  = 0;		//�ⲿ�����ź�
	
	/*REVI*/
	PD_DDR_DDR2 = 0;		//����PD2����
	PD_CR1_C12  = 0;		//��������
	PD_CR2_C22  = 0;		//�ⲿ�����ź�
}


/*****************************************************
** �������ƣ�void KeyCtrl_Process(void)
** �����������������ƽ���
******************************************************/
void KeyCtrl_Process(void)
{
  	uint8 Key_Signal = 0;
	uint16 Mode_Val = Menu_Param.stPARAM_List.Control_Mode.Val;
	
    Key_Signal = KeyCtrl_Scan();   
    IO_InPutCheck();
	
	//�жϲ˵��ź�
	if((Key_Signal & K_MENU) != 0)
		Menu_Signal = Key_Signal;
	
	//�жϵ���ź�
	if(Mode_Val == 1 || Mode_Val == 4)
	{
	  	if(((Key_Signal & K_MENU) == 0) && ((Key_Signal & 0xf0) == KEY_DOWN))
			Motor_Signal = Key_Signal & 0x0f;
	}
	else if(Mode_Val == 2)
	  	Motor_Signal = IO_InPutNoStop();
	else if(Mode_Val == 3)
	{
	  	Motor_Signal = IO_InPutWithStop();
		if(Key_Signal == (KEY_DOWN | K_Stop))
		  Motor_Signal = K_Stop;
	}
}




