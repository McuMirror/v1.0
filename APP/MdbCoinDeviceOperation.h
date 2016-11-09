/****************************************Copyright (c)***************************************************
 **                      JUNPENG Easivend Co.,Ltd.
 **                               http://www.easivend.com
 **-------------------------------------File Info--------------------------------------------------------
 ** @Filename:			ParaDef.c
 ** @brief:				The original version  
 ** @CreatedBy:			sunway 
 ** @CreatedDate:		2016-07-01
 ** @Version:			V1.0      
 *******************************************************************************************************/
#ifndef _MDBCOINDEVICEOPERATION_H
#define _MDBCOINDEVICEOPERATION_H

void MdbCoinResetAndSetup(void);
uint8_t CoinDevProcess(uint32_t *RecvMoney,unsigned char *CoinType,unsigned char *coinOptBack);
void MdbCoinGetTubeStatus(void);
void MdbCoinTypeEanbleOrDisable(unsigned char AcceptCtrl,unsigned char DispenCtrl);
uint32_t MdbGetCoinValue(unsigned char CoinTypeCredit);
unsigned char MdbCoinPayout(unsigned int PayoutValue,unsigned int *AcuPayout);
unsigned char ChangePayoutProcessLevel3(uint32_t PayMoney,uint32_t *PayoutMoney);

#endif
/**************************************End Of File*******************************************************/
