/****************************************Copyright (c)***************************************************
 **                      JUNPENG Easivend Co.,Ltd.
 **                               http://www.easivend.com
 **-------------------------------------File Info--------------------------------------------------------
 ** @Filename:			RTC.c
 ** @brief:				The original version  
 ** @CreatedBy:			sunway 
 ** @CreatedDate:		2016-07-01
 ** @Version:			V1.0      
 *******************************************************************************************************/
#include "..\API\API.h"
#include "..\API\OS.H"
#include "ParamDefine.h"
#include "MdbBillDeviceOperation.h"
/*********************************************************************************************************
** @APP Function name:   InitBillDeviceAndGetInfo
** @APP Input para:      None
** @APP Output para:     None
** @APP retrun para:     None
*********************************************************************************************************/
void MdbBillResetAndSetup(void)
{
	unsigned char i,err,MDBAck[36],MDBAckLen;
	err = 0x00;
	//Paramter
	memset((void *)MDBBillDevice.ManufacturerCode,0x00,3);
	memset((void *)MDBBillDevice.CurrencyCode,0x00,2);
	memset((void *)MDBBillDevice.BillSecurityLevel,0x00,2);
	memset((void *)MDBBillDevice.BillTypeEscrowEnable,0x00,16);
	memset((void *)MDBBillDevice.BillTypeCredit,0x00,16);
	memset((void *)MDBBillDevice.Level2Feature,0x00,4);
	//Poll,MEI-VN2672/ITL-NV9/ITL-NV11/CASHCODE/ICT
	API_SYSTEM_TimerChannelSet(0,400);
	while(API_SYSTEM_TimerReadChannelValue(0))
	{
		if(API_MDB_Poll_BillDevice(MDBAck,&MDBAckLen))
		{
			err = 0x01;
			break;
		}
		vTaskDelay(50);
	}
	if(err == 0x00)
	{
		//Poll err turn to Reset flow
		#ifdef APP_DBG_MDB_BILL
		Trace("InitBillDevice():Fst Poll ERR,turn to reset flow\r\n");
		#endif
		API_SYSTEM_TimerChannelSet(0,200);
		while(API_SYSTEM_TimerReadChannelValue(0))
		{
			if(API_MDB_Reset_BillDevice())
			{
				err = 0x01;
				break;
			}
			vTaskDelay(10);
		}
		if(err == 0x00)
		{
			MDBBillDevice.Oneline = 0x00;
			#ifdef APP_DBG_MDB_BILL
			Trace("InitBillDevice():Reset ERR\r\n");
			#endif
			return;
		}
		else
		{
			vTaskDelay(20);
			err = API_MDB_Poll_BillDevice(MDBAck,&MDBAckLen);
			if(err)
			{
				if((MDBAckLen >= 0x01) && (MDBAck[0] == 0x06))
				{
					#ifdef APP_DBG_MDB_BILL
					Trace("InitBillDevice():POLL to obtain JUST RESET OK\r\n");
					#endif
					MDBBillDevice.Oneline = 0x01;
				}
				else
				{
					#ifdef APP_DBG_MDB_BILL
					Trace("InitBillDevice():POLL Ok but ack ERR!(len = %d)\r\n",MDBAckLen);
					#endif
					MDBBillDevice.Oneline = 0x00;
					return;			
				}			
			}
			else
			{
				#ifdef APP_DBG_MDB_BILL
				Trace("InitBillDevice():POLL to obtain JUST RESET ERR!\r\n");
				#endif
				MDBBillDevice.Oneline = 0x00;
				return;
			}
		}
	}
	else
		MDBBillDevice.Oneline = 0x01;
	err = 0x00;
	//Setup
	API_SYSTEM_TimerChannelSet(0,500);
	while(API_SYSTEM_TimerReadChannelValue(0))
	{
		if(API_MDB_Setup_BillDevice(MDBAck,&MDBAckLen))
		{
			err = 0x01;
			break;
		}
	}
	if(err)
	{
		MDBBillDevice.Level = MDBAck[0];
		MDBBillDevice.CurrencyCode[0] = MDBAck[1];
		MDBBillDevice.CurrencyCode[1] = MDBAck[2];
		MDBBillDevice.ScalingFactor = (((unsigned int)MDBAck[3]) << 8) + MDBAck[4];
		MDBBillDevice.Decimal = MDBAck[5];
		MDBBillDevice.StackerCapacity = (((unsigned int)MDBAck[6]) << 8) + MDBAck[7];
		MDBBillDevice.BillSecurityLevel[0] = MDBAck[8];
		MDBBillDevice.BillSecurityLevel[1] = MDBAck[9];
		MDBBillDevice.Escrow = MDBAck[10];
		for(i=0;i<(MDBAckLen - 11);i++)
		{
			MDBBillDevice.BillTypeCredit[i] = MDBAck[11 + i];
		}
		#ifdef APP_DBG_MDB_BILL
		Trace("InitBillDevice() SETUP OK Info:\r\n");
		Trace("    Level = %d\r\n",MDBBillDevice.Level);
		Trace("    CurCode = %d%d\r\n",MDBBillDevice.CurrencyCode[0],MDBBillDevice.CurrencyCode[1]);
		Trace("    ScalingFactor = %d\r\n",MDBBillDevice.ScalingFactor);
		Trace("    Decimal = %d\r\n",MDBBillDevice.Decimal);
		Trace("    StackerCapacity = %d\r\n",MDBBillDevice.StackerCapacity);
		Trace("    BillSecurityLevel = %d%d\r\n",MDBBillDevice.BillSecurityLevel[0],MDBBillDevice.BillSecurityLevel[1]);
		Trace("    Escrow = %d\r\n",MDBBillDevice.Escrow);
		Trace("    BillType:");
		for(i=0;i<(MDBAckLen - 11);i++)
		{
			Trace("%02X ",MDBBillDevice.BillTypeCredit[i]);
		}
		Trace("\r\n");
		#endif
	}
	else
	{
		MDBBillDevice.Oneline = 0x00;
		#ifdef APP_DBG_MDB_BILL
		Trace("InitBillDevice():POLL ok but Setup ERR!\r\n");
		#endif
		return;
	}
	err = 0x00;
	//EXPANSION only level 2 support BillRecycSupport
	if((MDBBillDevice.Level > 0x02) || (MDBBillDevice.Level == 0x00))
	{
		MDBBillDevice.Oneline = 0x00;
		#ifdef APP_DBG_MDB_BILL
		Trace("InitBillDevice():unknowned Level\r\n");
		#endif
		return;	
	}
	if(MDBBillDevice.Level == 0x01)
	{
		#ifdef APP_DBG_MDB_BILL
		Trace("InitBillDevice():BillRecycSupport is not support for Device is Level 1\r\n");
		#endif
		MDBBillDevice.BillRecycSupport = 0x00;
	}
	if(MDBBillDevice.Level == 0x02)
	{
		API_SYSTEM_TimerChannelSet(0,500);
		while(API_SYSTEM_TimerReadChannelValue(0))
		{
			if(API_MDB_L2IdentificationWithOptBit_BillDevice(MDBAck,&MDBAckLen))
			{
				err = 0x01;
				break;
			}
		}
		if(err == 0x00)
		{
			#ifdef APP_DBG_MDB_BILL
			Trace("InitBillDevice():L2Identification ERR\r\n");
			#endif
			return;
		}
		else
		{
			if(MDBAckLen >= 33)
			{
				if(MDBAck[32] & 0x02)
				{
					#ifdef APP_DBG_MDB_BILL
					Trace("InitBillDevice():BillRecycSupport is support\r\n");
					#endif
					MDBBillDevice.BillRecycSupport = 0x01;
					MDBBillDevice.Level2Feature[0] = MDBAck[29];
					MDBBillDevice.Level2Feature[1] = MDBAck[30];
					MDBBillDevice.Level2Feature[2] = MDBAck[31];
					MDBBillDevice.Level2Feature[3] = MDBAck[32];
				}
				else
				{
					#ifdef APP_DBG_MDB_BILL
					Trace("InitBillDevice():BillRecycSupport is not support\r\n");
					#endif
					MDBBillDevice.BillRecycSupport = 0x00;
				}
			}
			else
				MDBBillDevice.BillRecycSupport = 0x00;
		}
	}
	//Enable Feature Level 2 only
	err = 0x00;
	if(MDBBillDevice.Level == 0x02)
	{
		API_SYSTEM_TimerChannelSet(0,100);
		while(API_SYSTEM_TimerReadChannelValue(0))
		{
			if(API_MDB_L2FeatureEnable_BillDevice((unsigned char *)MDBBillDevice.Level2Feature))
			{
				err = 0x01;
				break;
			}
			vTaskDelay(5);
		}
		#ifdef APP_DBG_MDB_BILL
		if(err)
			Trace("InitBillDevice():Enable feature ok\r\n");
		else
			Trace("InitBillDevice():Enable feature ERR\r\n");
		#endif
	}
	//Get Stacker status
	err = 0x00;
	API_SYSTEM_TimerChannelSet(0,200);
	while(API_SYSTEM_TimerReadChannelValue(0))
	{
		if(API_MDB_Stacker_BillDevice(MDBAck,&MDBAckLen))
		{
			err = 0x01;
			break;
		}
		vTaskDelay(10);
	}
	if(err)
	{
		if(MDBAckLen == 0x02)
		{
			#ifdef APP_DBG_MDB_BILL
			Trace("InitBillDevice():Get Stacker status Ok\r\n");
			#endif
			MDBBillDevice.StackerStatus = (MDBAck[0] & 0x80);
		}
		#ifdef APP_DBG_MDB_BILL
		else
			Trace("InitBillDevice():Get Stacker status ack ok but data err!\r\n");
		#endif
	}
	#ifdef APP_DBG_MDB_BILL
	else
		Trace("InitBillDevice():Get Stacker status ERR\r\n");
	#endif
	//Bill Type Enable
	err = 0x00;
	API_SYSTEM_TimerChannelSet(0,200);
	while(API_SYSTEM_TimerReadChannelValue(0))
	{
		if(API_MDB_BillType_BillDevice(0x003F,0x003F))
		{
			err = 0x01;
			break;
		}
		vTaskDelay(10);
	}
	if(err)
	{
		#ifdef APP_DBG_MDB_BILL
		Trace("InitBillDevice():Enable Bill type ok\r\n");
		#endif
	}
	#ifdef APP_DBG_MDB_BILL
	else
		Trace("InitBillDevice():Enable Bill type ERR\r\n");
	#endif
	/*
	//Recycler Setup
	err = 0x00;
	if(MDBBillDevice.BillRecycSupport)
	{
		API_SYSTEM_TimerChannelSet(0,200);
		while(API_SYSTEM_TimerReadChannelValue(0))
		{
			if(API_MDB_RecyclerSetup_BillDevice(MDBAck,&MDBAckLen))
			{
				err = 0x01;
				break;
			}
			vTaskDelay(10);
		}
		if(err)
		{
			MDBBillDevice.BillTpyeRoutedToRecycler = ((unsigned short)MDBAck[0] << 8) +  MDBAck[1];
			#ifdef APP_DBG_MDB_BILL
			Trace("InitBillDevice():Recycler setup ok\r\n");
			#endif
		}
		else
		{
			#ifdef APP_DBG_MDB_BILL
			Trace("InitBillDevice():Recycler setup ERR\r\n");
			#endif
			MDBBillDevice.BillTpyeRoutedToRecycler = 0x00;
			return;
		}
	}*/
}
/**************************************End Of File*******************************************************/
