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
void MdbCoinGetTubeStatus(void);
void MdbCoinTypeEanbleOrDisable(unsigned char AcceptCtrl,unsigned char DispenCtrl);
unsigned char MdbCoinPayout(unsigned int PayoutValue,unsigned int *AcuPayout);

#endif
/**************************************End Of File*******************************************************/
