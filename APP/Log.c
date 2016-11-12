/****************************************Copyright (c)****************************************************************************************************************************
**                      JUNPENG Easivend Co.,Ltd.
**                               http://www.easivend.com
**--------------File Info---------------------------------------------------------------------------------------------------------------------------------------------------------
** File name:           Maintenance.c
** Last modified Date:  2016-07-01
** Last Version:         
** Descriptions:        Uart0          
**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
** Created by:          sunway 
** Created date:        2016-07-01
** Version:             V1.0 
** Descriptions:        The original version       
***********************************************************************************************************************************************************************************/
#include "..\API\API.H"
#include "..\API\OS.H"
#include "ParamDefine.h"
#include "Log.h"
/*********************************************************************************************************
** Function name:       LogGetMoneyAPI
** Descriptions:        �ձ�ʱ������־
** input parameters:    InValueͶ�ҵĽ���Ϊ��λ,MoneyType=1Ӳ����,2ֽ����,3������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LogGetMoneyAPI(uint32_t InValue,uint8_t MoneyType)
{
	uint16_t money=(InValue/10);	
	//������ϸ���׼�¼
	if(MoneyType == 1)
		TotalTradeLog.TotalCoinPayIn += money;
	else if(MoneyType == 2)
		TotalTradeLog.TotalBillPayIn += money;
	
}

/*********************************************************************************************************
** Function name:       LogTransactionAPI
** Descriptions:        ���׳���ʱ������־
** input parameters:    type=1�����ɹ�,0����ʧ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LogTransactionAPI(int type)
{
	if(type==1)
		TotalTradeLog.TotalSuccesNumber++;	
	else
		TotalTradeLog.TotalErrorNumber++;		
}

/*********************************************************************************************************
** Function name:       LogChangeAPI
** Descriptions:        ����ʱ������־
** input parameters:    InValue����Ľ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LogChangeAPI(uint32_t InValue)
{	
	uint16_t money=(InValue/10);
	//���������׼�¼
	TotalTradeLog.TotalCoinPayout += money;
}

/*********************************************************************************************************
** Function name:       LogChangeAPI
** Descriptions:        ����ʱ������־
** input parameters:    InValue����Ľ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LogEndAPI()
{	
	LoadNewTotalLog();
}

/*********************************************************************************************************
** Function name:       LogClearAPI
** Descriptions:        ɾ��������־��¼
** input parameters:    
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LogClearAPI(void)
{
	TotalTradeLog.TotalCoinPayIn=0;
	TotalTradeLog.TotalBillPayIn=0;
	TotalTradeLog.TotalSuccesNumber=0;
	TotalTradeLog.TotalErrorNumber=0;	
	TotalTradeLog.TotalCoinPayout=0;
	LoadNewTotalLog();	
}

