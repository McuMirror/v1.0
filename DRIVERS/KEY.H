/****************************************Copyright (c)***************************************************
 **                      JUNPENG Easivend Co.,Ltd.
 **                               http://www.easivend.com
 **-------------------------------------File Info--------------------------------------------------------
 ** @Filename:			Key.c
 ** @brief:				The original version  
 ** @CreatedBy:			sunway 
 ** @CreatedDate:		2016-07-01
 ** @Version:			V1.0      
 *******************************************************************************************************/
#ifndef _KEY_H
#define _KEY_H

void KeyboardInit(void);
void KeyboardCtrl(unsigned char ctrl);
unsigned char ReadKeyValue(void);
void EINT1_IRQHandler(void);
unsigned char KeyBoardScan(void);
void KeyboardBacklightCtrl(unsigned char ctrl);

#endif
/**************************************End Of File*******************************************************/
