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
** Descriptions:        收币时更新日志
** input parameters:    InValue投币的金额分为单位,MoneyType=1硬币器,2纸币器,3读卡器
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogGetMoneyAPI(uint32_t InValue,uint8_t MoneyType)
{
	uint16_t money=(InValue/10);	
	//更新明细交易记录
	if(MoneyType == 1)
		TotalTradeLog.TotalCoinPayIn += money;
	else if(MoneyType == 2)
		TotalTradeLog.TotalBillPayIn += money;
	
}

/*********************************************************************************************************
** Function name:       LogTransactionAPI
** Descriptions:        交易出货时更新日志
** input parameters:    type=1出货成功,0出货失败
** output parameters:   无
** Returned value:      无
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
** Descriptions:        找零时更新日志
** input parameters:    InValue找零的金额
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogChangeAPI(uint32_t InValue)
{	
	uint16_t money=(InValue/10);
	//更新区域交易记录
	TotalTradeLog.TotalCoinPayout += money;
}

/*********************************************************************************************************
** Function name:       LogChangeAPI
** Descriptions:        找零时更新日志
** input parameters:    InValue找零的金额
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogEndAPI()
{	
	LoadNewTotalLog();
}

/*********************************************************************************************************
** Function name:       LogClearAPI
** Descriptions:        删除区域日志记录
** input parameters:    
** output parameters:   无
** Returned value:      无
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

