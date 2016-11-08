/****************************************Copyright (c)***************************************************
 **                      JUNPENG Easivend Co.,Ltd.
 **                               http://www.easivend.com
 **-------------------------------------File Info--------------------------------------------------------
 ** @Filename:			TIMER.H
 ** @brief:				The original version  
 ** @CreatedBy:			sunway 
 ** @CreatedDate:		2016-07-01
 ** @Version:			V1.0      
 *******************************************************************************************************/
#ifndef __TIMER_H 
#define __TIMER_H
typedef struct
{
	unsigned int MDBConversationTimer;
} SYSTEMTIMER;
extern volatile SYSTEMTIMER SystemTimer;
void InitTimer(unsigned char Numb,unsigned int Interval);
void SetSystemTimerValue(unsigned int *target,unsigned int value);
void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);
void TIMER2_IRQHandler(void);
void TIMER3_IRQHandler(void);
void TimerCtrl(unsigned char Numb,unsigned char ctrl);

#endif
/**************************************End Of File******************************************************/
