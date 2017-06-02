#include "iostm8s103f3.h"
#include "MenuData.h"
#include "MotorParam.h"
#include "MotorCtrl.h"
#include "MotorModeCtrl.h"
#include "Display.h"
#include "MenuCtrl.h"

#include "Eeprom.h"

uint8 EEPROM_Count = 0; //���ڵ�����
static uint8 Save_Mark = 0; //�����ݱ���ı�־λ

uint8 WordCount = 0;
uint16 *Point;

/******************************************************************************
*�������� static void EEPROM_UnLock(void)
*���ã�   EEProm����
*******************************************************************************/
static void EEPROM_UnLock(void){
  	FLASH_DUKR = 0xae;
	FLASH_DUKR = 0x56;
	while(!(FLASH_IAPSR&0x08));         //�ȴ����ܾ��� 
}

/******************************************************************************
*�������� void EEPROM_ChMark(void)
*���ã�   �����䶯��־
*******************************************************************************/
void EEPROM_ChMark(void){
    Save_Mark = 1;
}

/******************************************************************************
*�������� EEPROM_WordProgram(uint16 L_Half,uint16 H_Half)
*���ã�   �ִ洢 ��Ҫ����ɽ���
*******************************************************************************/
static void EEPROM_WordProgram(uint16 HalfWord)
{
    switch(WordCount)
    {
        case 0:
            //�ֱ��ʹ��
            FLASH_CR2_WPRG = 1;
            FLASH_NCR2_NWPRG = 0;  
            *Point++ = HalfWord;
            WordCount = 1;
        break;
        case 1:
            *Point++ = HalfWord;
            WordCount = 0;
            while(FLASH_IAPSR_EOP == 0);
        break;
        default:
            break;
    }
}

/******************************************************************************
*�������� EEPROM_EndProgram
*���ã�   EEPROM�����������β ���û������ ��0XFFFF
*******************************************************************************/
static void EEPROM_EndProgram(void)
{
    if(WordCount == 1)
        EEPROM_WordProgram(0xffff);
}

/******************************************************************************
*�������� EEPROM_SaveALL
*���ã�   EEPROM�������в���
*******************************************************************************/
static void EEPROM_SaveALL(void){
    uint8 i;
    
    Point = (uint16 *)EE_START;    
    WordCount = 0;  
	EEPROM_UnLock();
        
    //��ÿ��16λ�Ĳ˵�����ֵ
    for(i=0;i<LIST_Size;i++)
        EEPROM_WordProgram(Menu_Param.stList[i].Val);
    
    //����״̬
    EEPROM_WordProgram((uint16)Motor.Work_Sta);
    //�����ֱ��
    EEPROM_EndProgram();
    
    *((uint16*)EE_MARK) = EE_MARK_VAL;
    
    FLASH_IAPSR_DUL = 0;//DATA EEPROM����д����ʹ��  
}

/******************************************************************************
*�������� EEPROM_Read
*���ã�   ��ȡEEPROM����Ĳ���
*******************************************************************************/
static void EEPROM_Read(void){
    uint16 *p;
    uint8 i;
  
    p = (uint16 *)EE_START;    
    for(i=0;i<LIST_Size;i++)
  		Menu_Param.stList[i].Val = *p++; 
          
	MotorMode_SetCmd((WORK_STATUS)*p);	
	MotorParam_LockCtrl((BOOL)Menu_Param.stPARAM_List.Err_Report.Val);
	Menu_GetRadioSpdVal();
	Dis_SetNum(MotorParam_GetDisSpd(),0);
}

/******************************************************************************
*�������� EEPROM_Process
*���ã�   EEPROM�������
*******************************************************************************/
void EEPROM_Process(void){
  //�б��λ �Ҵ��ڶϵ� ����������
  if((++EEPROM_Count < 25) || (Save_Mark == 0)) return ;
  
  if(Save_Mark == 1)
  {
    EEPROM_Count = 0;
    Save_Mark = 0;	
    EEPROM_SaveALL();
  }
}

/******************************************************************************
*�������� EERPROM_Init
*���ã�   EEPROM��ʼ�� �ӿں���
*******************************************************************************/
void EERPROM_Init(void){
    if(*((uint16*)EE_MARK) != EE_MARK_VAL)  //EEPROM �ڲ�û������  �ǵ�һ����������
      EEPROM_SaveALL(); 
    else
      EEPROM_Read();
}