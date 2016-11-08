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
#include "MdbCoinDeviceOperation.h"
/*********************************************************************************************************
** @APP Function name:   MdbCoinResetAndSetup
** @APP Input para:      None
** @APP retrun para:     None
*********************************************************************************************************/
void MdbCoinResetAndSetup(void)
{
	unsigned char err,i,MDBAck[36],MDBAckLen;
	//Param
	memset((void *)MDBCoinDevice.ManufacturerCode,0x00,3);
	memset((void *)MDBCoinDevice.CurrencyCode,0x00,2);
	memset((void *)MDBCoinDevice.CoinTypeRouting,0x00,2);
	memset((void *)MDBCoinDevice.CoinTypeCredit,0x00,16);
	memset((void *)MDBCoinDevice.CoinTypeTubeStatus,0x00,16);
	memset((void *)MDBCoinDevice.CoinTypePresentInTube,0x00,16);
	//Poll
	err = 0x00;
	API_SYSTEM_TimerChannelSet(0,1000);
	while(API_SYSTEM_TimerReadChannelValue(0))
	{
		if(API_MDB_Poll_CoinDevice(MDBAck,&MDBAckLen))
		{
			err = 0x01;
			break;
		}
		vTaskDelay(10);
	}
	if(err == 0x00)
	{
		//Poll err turn to Reset flow
		API_SYSTEM_TimerChannelSet(0,1000);
		while(API_SYSTEM_TimerReadChannelValue(0))
		{
			if(API_MDB_Reset_CoinDevice())
			{
				err = 0x01;
				break;
			}
			vTaskDelay(10);
		}
		if(err == 0x00)
		{
			MDBCoinDevice.Oneline = 0x01;
			return;
		}
		else
		{
			vTaskDelay(100);
			err = API_MDB_Poll_CoinDevice(MDBAck,&MDBAckLen);
			if(err)
			{
				if((MDBAckLen >= 0x01) && (MDBAck[0] == 0x06))
				{
					#ifdef APP_DBG_MDB_COIN
					Trace("MdbCoinResetAndSetup():POLL to obtain JUST RESET OK\r\n");
					#endif
					MDBCoinDevice.Oneline = 0x01;
				}
				else
				{
					#ifdef APP_DBG_MDB_COIN
					Trace("MdbCoinResetAndSetup():POLL Ok but ack ERR!(len = %d)\r\n",MDBAckLen);
					#endif
					MDBCoinDevice.Oneline = 0x00;
					return;			
				}			
			}
			else
			{
				#ifdef APP_DBG_MDB_COIN
				Trace("MdbCoinResetAndSetup():POLL to obtain JUST RESET ERR!\r\n");
				#endif
				MDBCoinDevice.Oneline = 0x00;
				return;
			}
		}
	}
	else
		MDBCoinDevice.Oneline = 0x01;
	//SETUP
	err = 0x00;
	API_SYSTEM_TimerChannelSet(0,200);
	while(API_SYSTEM_TimerReadChannelValue(0))
	{
		if(API_MDB_Setup_CoinDevice(MDBAck,&MDBAckLen))
		{
			err = 0x01;
			break;
		}
		vTaskDelay(10);
	}
	if(err)
	{
		MDBCoinDevice.Level = MDBAck[0];
		MDBCoinDevice.CurrencyCode[0] = MDBAck[1];
		MDBCoinDevice.CurrencyCode[1] = MDBAck[2];
		MDBCoinDevice.ScalingFactor = MDBAck[3];
		MDBCoinDevice.Decimal = MDBAck[4];
		MDBCoinDevice.CoinTypeRouting[0] = MDBAck[5];
		MDBCoinDevice.CoinTypeRouting[1] = MDBAck[6];
		for(i=0;i<(MDBAckLen - 7);i++)
		{
			MDBCoinDevice.CoinTypeCredit[i] = MDBAck[7+i];
		}
		#ifdef APP_DBG_MDB_COIN
		Trace("MdbCoinResetAndSetup()--Coin Device Info:\r\n");
		Trace("    Level = %02d\r\n",MDBCoinDevice.Level);
		Trace("    CurCode = %02X%02X\r\n",MDBCoinDevice.CurrencyCode[0],MDBCoinDevice.CurrencyCode[1]);
		Trace("    ScalingFactor = %02d\r\n",MDBCoinDevice.ScalingFactor);
		Trace("    Decimal = %02d\r\n",MDBCoinDevice.Decimal);
		Trace("    CoinTypeRouting = %02X%02X\r\n",MDBCoinDevice.CoinTypeRouting[0],MDBCoinDevice.CoinTypeRouting[1]);
		for(i=0;i<16;i++)
		{
			Trace("    CoinType%dCredit = %02d\r\n",i,MDBCoinDevice.CoinTypeCredit[i]);
		}
		#endif
	}
	else
	{
		#ifdef APP_DBG_MDB_COIN
		Trace("MdbCoinResetAndSetup():Setup err\r\n");
		#endif
		MDBCoinDevice.Oneline = 0x00;
	}
	if(MDBCoinDevice.Level <= 0x02)
	{
		#ifdef APP_DBG_MDB_COIN
		Trace("MdbCoinResetAndSetup():software not support level 2 or lower level\r\n");
		#endif
		return;
	}
	//Expansion identification
	err = 0x00;
	API_SYSTEM_TimerChannelSet(0,200);
	while(API_SYSTEM_TimerReadChannelValue(0))
	{
		if(API_MDB_ExpanIdentification_CoinDevice(MDBAck,&MDBAckLen))
		{
			err = 0x01;
			break;
		}
		vTaskDelay(10);
	}
	if(err)
	{
		MDBCoinDevice.ManufacturerCode[0] = MDBAck[0];
		MDBCoinDevice.ManufacturerCode[1] = MDBAck[1];
		MDBCoinDevice.ManufacturerCode[2] = MDBAck[2];
		MDBCoinDevice.Feature[0] = MDBAck[0];
		MDBCoinDevice.Feature[1] = MDBAck[1];
		MDBCoinDevice.Feature[2] = MDBAck[2];
		MDBCoinDevice.Feature[3] = MDBAck[3];
		#ifdef APP_DBG_MDB_COIN
		Trace("MdbCoinResetAndSetup():identification Ok(ManuCode:%02X%02X%02X)\r\n",MDBAck[0],MDBAck[1],MDBAck[2]);
		#endif
	}
	else
	{
		#ifdef APP_DBG_MDB_COIN
		Trace("MdbCoinResetAndSetup():identification err\r\n");
		#endif
		return;
	}
	//Expansion Feature enable
	err = 0x00;
	API_SYSTEM_TimerChannelSet(0,200);
	while(API_SYSTEM_TimerReadChannelValue(0))
	{
		if(API_MDB_ExpanFeatureEnable_CoinDevice((void *)MDBCoinDevice.Feature))
		{
			err = 0x01;
			break;
		}
		vTaskDelay(10);
	}
	if(err)
	{
		#ifdef APP_DBG_MDB_COIN
		Trace("MdbCoinResetAndSetup():ExpanFeatureEnable Ok\r\n");
		#endif
	}
	#ifdef APP_DBG_MDB_COIN
	else
	{
		Trace("MdbCoinResetAndSetup():ExpanFeatureEnable err\r\n");
	}
	#endif
	//Expansion Send Diag status
	err = 0x00;
	err = API_MDB_ExpanDiagStatus_CoinDevice(&MDBAck[0],&MDBAck[1]);
	if(err)
	{
		#ifdef APP_DBG_MDB_COIN
		Trace("MdbCoinResetAndSetup():ExpanDiagStatus Ok\r\n");
		#endif
	}
	#ifdef APP_DBG_MDB_COIN
	else
	{
		Trace("MdbCoinResetAndSetup():ExpanDiagStatus err\r\n");
	}
	#endif
	return;
}
/*********************************************************************************************************
** @APP Function name:   MdbCoinGetTubeStatus
** @APP Input para:      None
** @APP retrun para:     None
*********************************************************************************************************/
void MdbCoinGetTubeStatus(void)
{
	unsigned char err,i,Ack[36],len;
	unsigned int status;
	err = 0x00;
	API_SYSTEM_TimerChannelSet(0,200);
	while(API_SYSTEM_TimerReadChannelValue(0))
	{
		if(API_MDB_TubeStatus_CoinDevice(Ack,&len))
		{
			err = 0x01;
			break;
		}
		vTaskDelay(10);
	}
	if(err)
	{
		status = (((unsigned int)Ack[0] << 8) + Ack[1]);
		for(i=0x00;i<(len - 2);i++)
		{
			MDBCoinDevice.CoinTypeTubeStatus[i] = (((status & (0x01 << i)) >> i) & 0x01);
			MDBCoinDevice.CoinTypePresentInTube[i] = Ack[2 + i];
		}
		#ifdef APP_DBG_MDB_COIN
		Trace("MdbCoinGetTubeStatus() Ok(%d):\r\n",len);
		for(err=0;err<16;err++)
		{
			Trace(" %d",MDBCoinDevice.CoinTypeTubeStatus[err]);
		}
		Trace("\r\n");
		for(err=0;err<16;err++)
		{
			Trace(" %d",MDBCoinDevice.CoinTypePresentInTube[err]);
		}
		Trace("\r\n");
		#endif
	}
	#ifdef APP_DBG_MDB_COIN
	else
	{
		Trace("MdbCoinGetTubeStatus():err\r\n");
	}
	#endif
}
/*********************************************************************************************************
** @APP Function name:   MdbCoinEanbleOrDisable
** @APP Input para:     None
** @APP retrun para:     None
*********************************************************************************************************/
void MdbCoinTypeEanbleOrDisable(unsigned char AcceptCtrl,unsigned char DispenCtrl)
{
	unsigned char Acmd,Dcmd;
	if(AcceptCtrl)
		Acmd = 0x07;
	else
		Acmd = 0x00;
	if(DispenCtrl)
		Dcmd = 0x07;
	else
		Dcmd = 0x00;
	API_SYSTEM_TimerChannelSet(0,200);
	while(API_SYSTEM_TimerReadChannelValue(0))
	{
		if(API_MDB_CoinType_CoinDevice(Acmd,Dcmd))
			break;
		vTaskDelay(10);
	}
}
/*********************************************************************************************************
** @APP Function name:   MdbCoinPayout
** @APP Input para:      None
** @APP retrun para:     None
*********************************************************************************************************/
unsigned char MdbCoinPayout(unsigned int PayoutValue,unsigned int *AcuPayout)
{
	unsigned char err,i,status,TypePayout[16],len,errNumber;
	unsigned int PayNumber,Paying,Payed;
	memset((void *)TypePayout,0x00,16);
	errNumber = 0x00;
	Paying = 0x00;
	Payed = 0x00;
	PayNumber = PayNumber / MDBCoinDevice.ScalingFactor;
	while(PayNumber > 0)
	{
		Paying = (PayNumber > 200)?200:PayNumber;
		err = API_MDB_ExpanPayout_CoinDevice(Paying);
		if(err)
		{
			errNumber = 0x00;
			PayNumber -= Paying;
			API_SYSTEM_TimerChannelSet(0,Paying * 20);
			while(API_SYSTEM_TimerReadChannelValue(0))
			{
				err = API_MDB_ExpanPayoutValuePoll_CoinDevice(&status);
				if(err == 0x01)
					break;
				vTaskDelay(30);
			}
			if(err == 0x01)
			{
				err = API_MDB_ExpanPayoutStatus_CoinDevice(TypePayout,&len);
				if(err)
				{
					for(i=0x00;i<len;i++)
					{
						Payed += ((TypePayout[i] * MDBCoinDevice.CoinTypeCredit[i]));
					}
				}
			}	
		}
		else
			errNumber++;
		if(errNumber >= 100)
		{
			err = 0x00;
			break;
		}
		vTaskDelay(20);
	}
	*AcuPayout = Payed;
	return err;
}
/********************************************************************************************************
** @TESTED COIN DEVICE:
** 	1.Panasonic:EUC 82M
** 	2.NRI:BV6-F-CNY
** 	3.MEI:CF 7000/8000
** 	4.FJ:FJM211CN
** @Operation Flow:
	1.CoinAcceptorChanger Initialization flow:
		1.1 RESET											--0x08
		1.2 POLL to obtain "Just Reset" response			--0x09
		1.3 EXPANSION IDENTIFICATION						--0x0F 0x00
		1.4 EXPANSION FEATURE ENABLE 						--0x0F 0x00
		1.5 EXPANSION SEND DIAG STATUS 						--0x0F 0x05
		1.6 TUBE STATUS										--0x0A
		1.7 COIN TYPE										--0x0C
		NOTE:A minimum 500 msec delay is required between a reset and the first TUBE STATUS command.
	2.CoinAcceptorChanger Payout flow(Level 3):
		2.1 Expansion Payout Coin							--0x0F 0x02
		2.2 Expansion Payout Value Poll						--0x0F 0x03
		2.3 Expansion Payout Status							--0x0F 0x04	
	3.CoinAcceptorChanger Poll flow:
		3.1 POLL											--0x09
	4.Note:This project is not support Level-1/Level-2 coin acceptor/changer.
*********************************************************************************************************/
/**************************************End Of File*******************************************************/
