#include "iostm8s103f3.h"
#include "SpdCalc.h"
#include "PwmGen.h"
#include "Eeprom.h"
#include "MenuData.h"
#include "String.h"


static SPEED_MOD SpdMod = SPEED_MOD_DEFAULT;
/****************************************************************************
** �������� ��static void TIM1_RegConfig (void)
** �������� ��TIM1�Ĵ�����ʼ��
****************************************************************************/
static void TIM1_RegConfig (void)
{
	HALF_WORD Tmp = {0};
	
  	CLK_PCKENR1 |= 1 << 7;				//TIM1ʱ��ʹ��
	
	TIM1_CR1_CEN = 0;					//�رն�ʱ������
	
	TIM1_CR1   = (1 << 7)|				//Ԥװ�ؼĴ�������
			     (0 << 5)|				//���ض���
			     (0 << 4)|				//���ϼ���
			     (0 << 3)|				//�ǵ�����ģʽ
			     (1 << 2)|				//���������ʱ�����ж�
			     (0 << 1);				//��������¼�
	
	TIM1_CR2   = 0;
	
	TIM1_SMCR  = 0;
	
	TIM1_IER   = (1 << 4)|				//������4�ж�
				 (1 << 3)|				//������3�ж�
			     (1 << 0);				//ʹ�ܸ����ж�
	
	TIM1_CCMR1 = (3 << 0);				//IC1ӳ����TRC��(Ϊ���������������ֹӲ���󴥷�)
	
	TIM1_CCMR3 = (7 << 4)|				//Fmaster/4,N = 8�����˲�(�˲�ʱ��128us)
				 (0 << 2)|				//������Ԥ��Ƶ
			     (1 << 0);				//IC3ӳ����TI3FP3��
	
	TIM1_CCMR4 = (5 << 4)|				//Fmaster/2,N = 8�����˲����˲�ʱ��64us��
				 (0 << 2)|				//������Ԥ��Ƶ
			     (1 << 0);				//IC4ӳ����TI4FP4��
	
	TIM1_CCER1 = (1 << 0);				//����ʹ��
	
	TIM1_CCER2 = (1 << 5)|				//IC4���½��ز���
				 (1 << 4)|				//IC4����ʹ��
				 (1 << 1)|				//IC3���½��ز���
				 (1 << 0);				//IC3����ʹ��
	
	TIM1_PSCRH = 0;						//64��Ƶ
	TIM1_PSCRL = 63;
	
	Tmp.HalfWord = TIM1_ARR_VALUE;		//Ԥװ��ֵ
	TIM1_ARRH = Tmp.BYTE.HByte;	
	TIM1_ARRL = Tmp.BYTE.LByte;	
	
	TIM1_EGR_UG = 1;					//���¼Ĵ���
	
	TIM1_CR1_CEN = 1;					//������ʱ������
}

/****************************************************************************
** �������� ��static void TIM1_IOConfig (void)
** �������� ��TIM1��IO�ڳ�ʼ��
****************************************************************************/
static void TIM1_IOConfig (void)
{
  	PC_DDR_DDR3 = 0;		//����PC3ΪSPDIN���������룩
	PC_CR1_C13  = 0;		//��������
	PC_CR2_C23  = 0;		//��ֹ�ⲿ�ж�
	
	/*SZERO*/
	PC_DDR_DDR4 = 0;		//����PC4Ϊ����
	PC_CR1_C14  = 1;		//��������
	PC_CR2_C24  = 0;		//��ֹ�ⲿ�ж�
}

/****************************************************************************
** �������� ��void SpdCalc_Init (void)
** �������� �������ٶ������ʼ��
****************************************************************************/
void SpdCalc_Init (void)
{
	TIM1_IOConfig();
	TIM1_RegConfig();
}

/****************************************************************************
** �������� ��uint16 Filter_1st(uint16 OldData,uint16 NewData)
** �������� ��һ���˲�
****************************************************************************/
uint16 Filter_1st(uint16 OldData,uint16 NewData)
{
	uint16 usResult;
	
	if(NewData < OldData) usResult = OldData - ((OldData - NewData) * SPD_K  + 128)/ 256;
	else usResult = OldData + ((NewData - OldData) * SPD_K + 128)/ 256;
	
	if(usResult < 5) usResult = 0;
	return usResult;
}

/****************************************************************************
** �������� ��void SpdCalc_Process (void)
** �������� �������ٶ�����
****************************************************************************/
void SpdCalc_Process (void)
{
	HALF_WORD CapTmp = {0};
	uint16 SpdTmp = 0;
    static uint16 FdbNum = 0;
    
    if(FdbNum != Menu_Param.stPARAM_List.SpdFdb.Val)
    {
        FdbNum = Menu_Param.stPARAM_List.SpdFdb.Val;
        SpdMod.FdbSpdSum = 0;
        SpdMod.FdbCnt = 0;
        
        memset(SpdMod.FdbSpd,0,sizeof(SpdMod.FdbSpd));
    }

	TIM1_EGR_CC1G = 1;
	SpdTmp = (SpdMod.CapValue == 0) ? 0 : SPEED_CVT(SpdMod.CapValue);   //���㷴���ٶ�
	
	/*���ٶȷ���������ǰ��Ԥ�ȹ��㷴���ٶ�*/
	if (SpdMod.CapEnble == 1)
	{
		CapTmp.BYTE.HByte = TIM1_CCR1H;
		CapTmp.BYTE.LByte = TIM1_CCR1L; //��һ�β�����ٶ�
		TIM1_SR1_CC1IF = 0;
		
		if (CapTmp.HalfWord > MIN_CAPVALUE)
			SpdTmp = (SPEED_CVT(CapTmp.HalfWord) < SpdTmp) ? SPEED_CVT(CapTmp.HalfWord) : SpdTmp;
	}
	
	/*�����ٶȻ����˲��㷨*/
	SpdMod.FdbSpdSum += SpdTmp;
	SpdMod.FdbSpdSum -= SpdMod.FdbSpd[SpdMod.FdbCnt];
	
	SpdMod.FdbSpd[SpdMod.FdbCnt++] = SpdTmp;
	SpdMod.FdbCnt = (SpdMod.FdbCnt >= FdbNum) ? 0 : SpdMod.FdbCnt;
	
	SpdMod.CurSpd = SpdMod.FdbSpdSum/FdbNum;
	
	/*��ʾ�ٶȽ���һ���˲�*/
	SpdMod.DisSpd = Filter_1st(SpdMod.DisSpd , SpdMod.CurSpd);
}

/****************************************************************************
** �������� ��uint16 SpdCalc_GotSpd (void)
** �������� ����ȡ�����ٶ�
****************************************************************************/
uint16 SpdCalc_GotSpd (void)
{
	return SpdMod.CurSpd;
}

/****************************************************************************
** �������� ��uint16 SpdCalc_GotDisSpd (void)
** �������� ����ȡ��ʾ���ٶ�
****************************************************************************/
uint16 SpdCalc_GotDisSpd(void)
{
  	return SpdMod.DisSpd;
}

/****************************************************************************
** �������� ��__interrupt void TIM1_OVR_IRQHandler (void)
** �������� ��TIM1����жϴ���
****************************************************************************/
#pragma vector = TIM1_OVR_UIF_vector
__interrupt void TIM1_OVR_IRQHandler (void)
{
	SpdMod.CapValue = 0;	
	SpdMod.CapEnble = 0;	
	
	TIM1_SR1_UIF = 0;
}

/****************************************************************************
** �������� ��__interrupt void TIM1_CAPCOM_CC3IF_IRQHandler (void)
** �������� ��TIM1�����жϴ���
****************************************************************************/
#pragma vector = TIM1_CAPCOM_CC3IF_vector
__interrupt void TIM1_CAPCOM_CC3IF_IRQHandler (void)
{
	HALF_WORD CapTmp = {0};
	
	//�ٶȷ����жϴ���
	if (TIM1_SR1_CC3IF)
	{
		if (SpdMod.CapEnble == 1) 
		{
			CapTmp.BYTE.HByte  = TIM1_CCR3H;
			CapTmp.BYTE.LByte  = TIM1_CCR3L;
			
			if (CapTmp.HalfWord > MIN_CAPVALUE)
			{
				if (CapTmp.HalfWord < SpdMod.CapValue)
				{
					//���������������ֵ��С��ǰһ��ֵ��һ��
					if ((SpdMod.CapValue - CapTmp.HalfWord) < SpdMod.CapValue/2)	
					{
						SpdMod.CapErrCnt = 0;
						SpdMod.CapTmp = 0;
						SpdMod.CapValue = CapTmp.HalfWord;
					}
					else
					{
						//Ϊ��ֹ�����ʵ����ֵ���������źŴ������Ҫ�Ը����źŽ���4��ȡ���������ֵ������ʵֵ
						if (SpdMod.CapErrCnt < 3) 
						{
							SpdMod.CapTmp = (CapTmp.HalfWord > SpdMod.CapTmp) ? CapTmp.HalfWord : SpdMod.CapTmp;
							SpdMod.CapErrCnt++;
						}
						else
						{
							SpdMod.CapErrCnt = 0;
							SpdMod.CapValue = SpdMod.CapTmp;
							SpdMod.CapTmp = 0;
						}
					}
				}
				else 
				{
					SpdMod.CapErrCnt = 0;
					SpdMod.CapTmp = 0;
					SpdMod.CapValue = CapTmp.HalfWord;
				}
			}
			else 
			{
				SpdMod.CapErrCnt = 0;
				SpdMod.CapTmp = 0;
			}
		}
		else 
		{
			SpdMod.CapErrCnt = 0;
			SpdMod.CapTmp = 0;
			
			//����һ����ʼ��ת�ٲ���ֵ��ʹ�������������ʱ����
			SpdMod.CapValue = INIT_CAPVALUE;	
			SpdMod.CapEnble = 1;
		}
		
		TIM1_EGR_UG    = 1;
		TIM1_SR1_CC3IF = 0;
	}
	
	//�����ź��жϴ���
	if (TIM1_SR1_CC4IF)
	{
        EEPROM_Count = 0;
		if (PwmGen_GotTRIGSW() == EN_TRIGO) PwmGen_GenTRIGO();
		TIM1_SR1_CC4IF = 0;
	}
}





