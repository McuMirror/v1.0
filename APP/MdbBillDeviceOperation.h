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
#ifndef _MDBBILLDEVICEOPERATION_H
#define _MDBBILLDEVICEOPERATION_H

void MdbBillResetAndSetup(void);
uint8_t BillDevProcess(uint32_t *RecvMoney,unsigned char *BillType,unsigned char billOpt,unsigned char *billOptBack);
void MdbBillGetStackerStatus(void);
void MdbBillStackBillOrReturn(unsigned char ctrl);
void MdbBillTypeEscrowEanbleOrDisable(unsigned short EscrowCtrl);
void MdbBillTypeEanbleOrDisable(unsigned short AcceptCtrl);
unsigned char MdbRecyclerPayout(unsigned int PayoutValue,unsigned int *AcuPayout);

#endif
/**************************************End Of File*******************************************************/
