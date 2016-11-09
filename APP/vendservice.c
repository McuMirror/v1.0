/****************************************Copyright (c)*************************************************
**                      JUNPENG Easivend Co.,Ltd.
**                               http://www.easivend.com
**--------------File Info------------------------------------------------------------------------------
** File name:           uart0.c
** Last modified Date:  2016-07-01
** Last Version:         
** Descriptions:        Uart0          
**------------------------------------------------------------------------------------------------------
** Created by:          sunway 
** Created date:        2016-07-01
** Version:             V1.0 
** Descriptions:        The original version
********************************************************************************************************/
#include "..\API\API.H"
#include "..\API\OS.H"
#include "ParamDefine.h"
#include "MdbBillDeviceOperation.h"
#include "MdbCashlessDeviceOperation.h"
#include "MdbCoinDeviceOperation.h"
#include "Menu.h"
unsigned char key;
uint8_t  billOpt = 0,billOptBack = 0;
uint32_t InValue = 0;
uint8_t Billtype = 0;
uint32_t SumValue = 0;
char    *pstr;
char	strMoney[10];
static void VendingService(void);
static void VendingIdle(void);
/*********************************************************************************************************
** @API Function name:   InitVmcParam
** @API Input para:      None
** @API retrun para:     None
*********************************************************************************************************/
void VendService(void *pvParameters)
{
	//unsigned char err,err0,Mdback[36],MdbackLen;
	//unsigned int CoinPayin = 0x00,BillPayin = 0x00;
	//unsigned int CoinPayout = 0x00,BillPayout = 0x00;
	//unsigned char Temp[1024];
	//CoinPayout = CoinPayout;
	//CoinPayin = CoinPayin;
	//BillPayin = BillPayin;
	//BillPayout = BillPayout;
	pvParameters = pvParameters;
	API_SYSTEM_PinFuctionSelect();
	API_FRAM_Init();
	API_SYSTEM_TimerInit(0);
	API_SYSTEM_TimerInit(1);
	API_COM_Init(0,115200);
	API_MDB_UartInit();
	API_LCM_Init();
	API_KEY_KeyboardInit();
	API_BUZZER_Buzzer();
	//vTaskDelay(2000);
	InitVmcParam();
	InitTradeParam();
	//MdbCoinResetAndSetup();
	//MdbCoinGetTubeStatus();
	//MdbCoinTypeEanbleOrDisable(0x01,0x01);
	//MdbBillResetAndSetup();
	//Trace("MDBBillDevice.Oneline = %d\r\n",MDBBillDevice.Oneline);
	//VMCParam.Language = 0x00;
	VMCParam.VMCMode = VMC_MODE_VEND;
	/*
	while(1)
	{
		if(API_COM_RecBufferIsNotEmpty(0))
		{
			err = API_COM_ReadCharFromBuffer(0);
			if(err == 'C')
				Trace("@@@%d\r\n",API_FRAM_WriteFram(VMC_FRAM_ADDR,0x00,Temp,1024));
		}
		Trace("Idel.....\r\n");
		vTaskDelay(500);
	}
	while(1)
	{
		err = API_MDB_Poll_CoinDevice(Mdback,&MdbackLen);
		if(err == 0x01)
		{
			if(MdbackLen == 0x01)
			{
				if(Mdback[0] == 0x01)
				{
					Trace("\r\n***Start Payout %d\r\n",CoinPayin);
				}
			}
		}
		err= API_MDB_Poll_BillDevice(Mdback,&MdbackLen);
		if(err == 0x01)
		{
			if(MdbackLen == 0x01)
			{
				if(Mdback[0] & 0x80)
				{
					Trace("Bill Routing:%X\rn",(Mdback[0] & 0x70) >> 4);
					Trace("Bill Type:%d\r\n",(Mdback[0] & 0x0f));
					err0 = API_MDB_Escrow_BillDevice(0x01);
					if(err0)
						Trace("Escrow Ok\r\n");
					else
						Trace("Escrow err\r\n");
				}
				else
				{
					Trace("Status:Len = %d,data = %02X\r\n",MdbackLen,Mdback[0]);
				}
			}
			if(MdbackLen >= 0x02)
			{
				Trace("Len = %d,data0 = %02X,data1 = %02X\r\n",MdbackLen,Mdback[0],Mdback[1]);
			}	
		}
		vTaskDelay(100);
	}

	while(1)
	{
		err = API_MDB_Poll_CoinDevice(Mdback,&MdbackLen);
		if(err == 0x01)
		{
			if(MdbackLen == 0x01)
			{
				if(Mdback[0] == 0x01)
				{
					Trace("\r\n***Start Payout %d\r\n",CoinPayin);
					MdbCoinTypeEanbleOrDisable(0x00,0x01);
					err = MdbCoinPayout(CoinPayin,&CoinPayout);
					Trace("err = %d,CoinPayout = %d\r\n",err,CoinPayout * MDBCoinDevice.ScalingFactor);
					MdbCoinGetTubeStatus();
					Trace("***END CoinPayout\r\n");
					MdbCoinTypeEanbleOrDisable(0x01,0x01);
					CoinPayin = 0x00;
					CoinPayout = 0x00;
				}
			}
			else if(MdbackLen == 0x02)
			{
				if(Mdback[0] & 0x40)
				{
					if(((Mdback[0] & 0x30) >> 4) <= 0x01)
					{
						CoinPayin += (MDBCoinDevice.CoinTypeCredit[Mdback[0] & 0x0F] * MDBCoinDevice.ScalingFactor);
						Trace("@%d\r\n",CoinPayin);
						MdbCoinGetTubeStatus();
					}
				}
			}
		}
		vTaskDelay(50);
	}
	*/
	while(1)
	{
		switch(VMCParam.VMCMode)
		{
			case VMC_MODE_VEND:
				VendingService();
				break;
			case VMC_MODE_MAIN:
				VendingIdle();
				break;
		}
	}
}
/*********************************************************************************************************
** @API Function name:   VendingService
** @API Input para:      None
** @API retrun para:     None
*********************************************************************************************************/
static void VendingService(void)
{
	/*
	unsigned char err,err0,Mdback[36],MdbackLen;
	unsigned int CoinPayin = 0x00,BillPayin = 0x00;
	unsigned int CoinPayout = 0x00,BillPayout = 0x00;
	CoinPayout = CoinPayout;
	CoinPayin = CoinPayin;
	BillPayin = BillPayin;
	BillPayout = BillPayout;
	*/
	LPC_GPIO2->FIODIR |= 1ul <<12;
	API_LCM_ClearScreen();

	vTaskDelay(100);
	//³õÊ¼»¯Ó²±ÒÆ÷
	//if(VMCParam.MdbCoinDeviceEAB == 0x00)
	//{
	//	API_LCM_Printf(0,0,0,0,"CoinDevice is disable by system.\r\n");
	//}
	//else
	{
		MdbCoinResetAndSetup();
		if(MDBCoinDevice.Oneline)
			API_LCM_Printf(0,0,0,0,"CoinDevice is Ok.\r\n");
		else
			API_LCM_Printf(0,0,0,0,"CoinDevice is err.\r\n");
		MdbCoinTypeEanbleOrDisable(1,1);
		vTaskDelay(20);
		CoinDevProcess(&InValue,&Billtype,&billOptBack);
		vTaskDelay(20);
		//MdbCoinPayout(150,&InValue);
		//Trace("Drvchange=%ld\r\n",InValue);
	}
	//if(VMCParam.MdbBillDeviceEAB == 0x00)
	//{
	//	API_LCM_Printf(0,0,0,0,"BillDevice is disable by system.\r\n");
	//}
	//else
	{
		MdbBillResetAndSetup();
		if(MDBBillDevice.Oneline)
			API_LCM_Printf(0,0,0,0,"BillDevice is Ok.\r\n");
		else
			API_LCM_Printf(0,0,0,0,"BillDevice is err.\r\n");
		BillDevProcess(&InValue,&Billtype,MBOX_BILLENABLEDEV,&billOptBack);
		vTaskDelay(20);
		BillDevProcess(&InValue,&Billtype,0,&billOptBack);
		vTaskDelay(20);
	}
	
	
	API_LCM_Printf(60,6,0,0,UIMenu.welcome[VMCParam.Language]);
	while(1)
	{
		if(API_KEY_ReadKey() == 'M')
		{
			VMCParam.VMCMode = VMC_MODE_MAIN;
			break;
		}
		vTaskDelay(10);
		API_RTC_Read((void *)Rtc);
		API_LCM_Printf(40,8,0,0,"%04D/%02D/%02D %02D:%02D:%02D",(((unsigned int)Rtc[5] << 8) + Rtc[6]),Rtc[4],Rtc[3],Rtc[2],Rtc[1],Rtc[0]);
		if(API_COM_RecBufferIsNotEmpty(0x00))
		{
			Rtc[0] = 0x00;
			Rtc[1] = 34;
			Rtc[2] = 19;
			Rtc[3] = 30;
			Rtc[4] = 8;
			Rtc[5] = 2016 / 256;
			Rtc[6] = 2016 % 256;
			API_RTC_Write((void *)Rtc);
		}
	}
}
/*********************************************************************************************************
** @API Function name:   VendingIdle
** @API Input para:      None
** @API retrun para:     None
*********************************************************************************************************/
static void VendingIdle(void)
{
	LPC_GPIO2->FIOSET |= 1ul <<12;
	vTaskDelay(50);	
	LPC_GPIO2->FIOCLR |= 1ul <<12;
	vTaskDelay(50);
}
/**************************************End Of File*******************************************************/
