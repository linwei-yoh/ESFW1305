#include "iostm8s103f3.h"
#include "string.h"
#include "ADC.h"



static ADC_STRUCT ADC1 = ADC_STRUCT_DEFAULT;
/****************************************************************************
** �������� ��static void ADC_IOConfig (void)
** �������� ��ADC��IO�ڳ�ʼ��
****************************************************************************/
static void ADC_IOConfig (void)
{	
	PD_DDR_DDR3 = 0;				//����PD3ΪADIN4
	PD_CR1_C13  = 0;				//��������
	PD_CR2_C23  = 0;				//��ֹ�ⲿ�ж�	
	ADC_TDRL   |= 1 << 4;			//��ֹʩ���ش����������͹���	
}

/****************************************************************************
** �������� ��static void ADC_RegConfig (void)
** �������� ��ADC�Ĵ�����ʼ��
****************************************************************************/
static void ADC_RegConfig (void)
{	
	CLK_PCKENR2 |= 1 << 3;			//����ADCʱ��						
	
	ADC_CR1_ADON = 0;				//�ر�ADC
	
	ADC_CSR = (0 << 7)|				//���EOC��־
			  (1 << 5)|				//ʹ��EOC�¼��ж�
			  (0 << 4);				//��ֹADC���Ź��ж�
	
	ADC_CR1 = (2 << 4)|				//Fadc = Fmaster/4
			  (0 << 1);				//����ת��ģʽ
	
	ADC_CR2 = (0 << 6)|				//��ֹ�ⲿ����ת��
			  (1 << 3)|				//�����Ҷ���
			  (0 << 1);				//��ֹɨ��ģʽ
	
	ADC_CR3 = 0 << 7;				//���ݻ����ֹ
	
	ADC_CSR_CH   = 4;				//ת��ͨ��ѡ��AIN4
	
	ADC_CR1_ADON = 1;				//��ADC
}

/****************************************************************************
** �������� ��void ADC_Init (void)
** �������� ��ADC��ʼ��
****************************************************************************/
void ADC_Init (void)
{
	ADC_IOConfig();
	ADC_RegConfig();
}

/****************************************************************************
** �������� ��static uint8 ADC_TurnOn (void)
** �������� ������ADCת��
** ����ֵ   ��1�������ɹ�  0��ADCģ����æ 
****************************************************************************/
static uint8 ADC_TurnOn (void)
{
	if (ADC1.Status != AD_Idle) return 0;	//ֻ��ADCΪ����ʱ��������ת��
	
	ADC1.Status  = AD_Working;	
	ADC_CR1_ADON = 1;
	return 1;
}

/****************************************************************************
** �������� ��void ADC_Process (void)
** �������� ��ADCת������
****************************************************************************/
void ADC_Process (void)
{
	uint16 ADCTmp = (ADC1.CvtValue < ADC_CVTMIN) ? 0UL : ((ADC1.CvtValue > ADC_CVTMAX) ? 1024UL : ADC1.CvtValue);	
		
	/*���û����˲��㷨����ADC����ֵ*/	 
	ADC1.CvtSum += ADCTmp;
	ADC1.CvtSum -= ADC1.CvtRes[ADC1.CvtCnt];
	
	ADC1.CvtRes[ADC1.CvtCnt++] = ADCTmp;
	ADC1.CvtCnt = (ADC1.CvtCnt >= ADCBUF_NUM) ? 0 : ADC1.CvtCnt;
	
	ADC1.CalcRes = ADC1.CvtSum/ADCBUF_NUM;
		
	ADC_TurnOn();		
}

/****************************************************************************
** �������� ��uint16 ADC_GotCvtRes (void)
** �������� ����ȡADCת�����
****************************************************************************/
uint16 ADC_GotCvtRes (void)
{
	return ADC1.CalcRes;
}

/****************************************************************************
** �������� ��__interrupt void ADC_EOC_IRQHandler (void)
** �������� ��ADCת���жϴ���
****************************************************************************/
#pragma vector = ADC1_EOC_vector
__interrupt void ADC_EOC_IRQHandler (void)
{
	HALF_WORD Data;
	
	/*����ADCת�����*/
	Data.BYTE.LByte = ADC_DRL;
	Data.BYTE.HByte = ADC_DRH;
	ADC1.CvtValue   = Data.HalfWord;
		
	ADC1.Status = AD_Idle;
	ADC_CSR_EOC = 0;	
}




