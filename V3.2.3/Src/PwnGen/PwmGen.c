#include "iostm8s103f3.h"
#include "PwmGen.h"


/****************************************************************************
						      ������������
****************************************************************************/
typedef enum {PwmIdle = 0,PwmDly,PwmTrig} PWM_STATUS;

/****************************************************************************
						      �ֲ���������
****************************************************************************/
static PWM_STATUS PwmSta = PwmIdle;

static uint8  TRIG_SW = DIS_TRIGO;
static uint8  TRIG_DIR = 0;
static uint16 TirgDly = 0;

/****************************************************************************
** �������� ��static void TIM2_RegConfig (void)
** �������� ��TIM2�Ĵ�����ʼ��
****************************************************************************/
static void TIM2_RegConfig (void)
{
	HALF_WORD ARRTmp = {0};
	
  	CLK_PCKENR1 |= 1 << 5;				//TIM2ʱ��ʹ��
	
	TIM2_CR1_CEN = 0;					//�رն�ʱ������
	
	TIM2_CR1   = (1 << 7)|				//Ԥװ�ؼĴ�������
			     (1 << 3)|				//������ģʽ
			     (1 << 2)|				//���������ʱ�����ж�
			     (0 << 1);				//��������¼�
	
	TIM2_IER   = (1 << 0);				//��������ж�
	
	TIM2_PSCR  = 4;						//16��Ƶ
	
	ARRTmp.HalfWord = TIM2_ARR_VALUE;	//Ԥװ��ֵ
	TIM2_ARRH  = ARRTmp.BYTE.HByte;	
	TIM2_ARRL  = ARRTmp.BYTE.LByte;	
	
	TIM2_EGR_UG = 1;					//���¼Ĵ���
	
	TIM2_CCMR1 = 0;
	TIM2_CCMR2 = 0;
	TIM2_CCMR3 = 0;
	TIM2_CCER1 = 0;	
	TIM2_CCER2 = 0;				
}

/****************************************************************************
** �������� ��static void TIM2_IOConfig (void)
** �������� ��TIM2��IO�ڳ�ʼ��
****************************************************************************/
static void TIM2_IOConfig (void)
{	
	/*FWDO*/
	PC_ODR_ODR5 = 0;		//�ر�FWDO���
	PC_DDR_DDR5 = 1;		//����PC5Ϊ���
	PC_CR1_C15  = 1;		//�������
	PC_CR2_C25  = 0;		//�������Ϊ2MHz
	
	/*REVO*/
	PC_ODR_ODR6 = 0;		//�ر�REVO���
	PC_DDR_DDR6 = 1;		//����PC6Ϊ���
	PC_CR1_C16  = 1;		//�������
	PC_CR2_C26  = 0;		//�������Ϊ2MHz
}

/****************************************************************************
** �������� ��void PwmGen_Init (void)
** �������� ���ɿع败�������ʼ��
****************************************************************************/
void PwmGen_Init (void)
{
	TIM2_IOConfig();
	TIM2_RegConfig();
}

/*****************************************************************************
** �������ƣ�void IOCtrl_TrunOnTRIAC (void)
** ��������������FWDO���
*****************************************************************************/
static void PwmGen_TrunOnTRIAC (void)
{
  	if(TRIG_DIR == 0)
		PC_ODR_ODR5 = 1;
	else
		PC_ODR_ODR6 = 1;
}

/*****************************************************************************
** �������ƣ�void IOCtrl_TrunOffTRIAC (void)
** �����������ر�FWDO���
*****************************************************************************/
static void PwmGen_TrunOffTRIAC (void)
{
	PC_ODR_ODR5 = 0;
	PC_ODR_ODR6 = 0;
}

/*****************************************************************************
** �������ƣ�void PwmGen_EnableTRIGO (void)
** ������������������ɿع败������
*****************************************************************************/
void PwmGen_EnableTRIGO (void)
{
	TRIG_SW = EN_TRIGO;
}

/*****************************************************************************
** �������ƣ�void PwmGen_DisableTRIGO (void)
** ������������ֹ�����ɿع败������
*****************************************************************************/
void PwmGen_DisableTRIGO (void)
{
	TRIG_SW = DIS_TRIGO;
}

/*****************************************************************************
** �������ƣ�uint8 PwmGen_GotTRIGSW (void)
** ������������ȡ�������忪��״̬
*****************************************************************************/
uint8 PwmGen_GotTRIGSW (void)
{
	return TRIG_SW;
}

/*****************************************************************************
** �������ƣ�void PwmGen_SetTRIGDir (uint8 Dir)
** �������������ÿɿع败��ת��
*****************************************************************************/
void PwmGen_SetTRIGDir (uint8 Dir)
{
	TRIG_DIR = Dir;
}

/*****************************************************************************
** �������ƣ�void PwmGen_SetTRIGStren (int32 Stren)
** �������������ÿɿع败��������
*****************************************************************************/
void PwmGen_SetTRIGStren (int32 Stren)
{
	uint32 Ratio = _IQsat(Stren, 4096L, 0L); 
	
	TirgDly = MAX_TRIG_DLY - (MAX_TRIG_DLY - MIN_TRIG_DLY)*Ratio/4096UL;
}

/****************************************************************************
** �������� ��void PwmGen_GenTRIGO (void)
** �������� ������ɿع败������
****************************************************************************/
void PwmGen_GenTRIGO (void)
{
	HALF_WORD Tmp = {0};
	
	PwmGen_TrunOffTRIAC();
	Tmp.HalfWord = _IQsat(TirgDly, MAX_TRIG_DLY, MIN_TRIG_DLY);
	TIM2_ARRH    = Tmp.BYTE.HByte;
	TIM2_ARRL    = Tmp.BYTE.LByte;
	TIM2_EGR_UG  = 1;

	PwmSta = PwmDly;
	
	Tmp.HalfWord = TRIG_WIDTH;
	TIM2_ARRH    = Tmp.BYTE.HByte;
	TIM2_ARRL    = Tmp.BYTE.LByte;
	
	TIM2_CR1_CEN = 1;
}

/****************************************************************************
** �������� ��__interrupt void TIM2_OVR_IRQHandler (void)
** �������� ��TIM2����жϴ���
****************************************************************************/
#pragma vector = TIM2_OVR_UIF_vector
__interrupt void TIM2_OVR_IRQHandler (void)
{		
	if (PwmSta == PwmDly)
	{
		PwmGen_TrunOnTRIAC();
		PwmSta = PwmTrig;
		TIM2_CR1_CEN = 1;
	}
	else
	{
		PwmGen_TrunOffTRIAC();
		PwmSta = PwmIdle;
	}
	
	TIM2_SR1_UIF = 0;
}


