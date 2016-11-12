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
#include "Log.h"
#define VMC_FREE		  0                         //空闲状态
#define VMC_CHAXUN	      1							//货道查询状态
#define VMC_CHAXUNHEFANG  2							//盒饭柜号查询状态
#define VMC_SALE 	      3							//交易状态
#define VMC_OVERVALUE	  4							//面值太大
#define VMC_XUANHUO    	  5							//选货
#define VMC_WUHUO    	  6							//无货
#define VMC_LESSMONEY 	  7							//钱不够商品单价
#define VMC_CHANGESHORT	  8							//零钱不够
#define VMC_READVENDFAIL  9							//读卡器扣款失败
#define VMC_CHUHUO  	  10						//出货
#define VMC_QUHUO  	  	  11						//取货
#define VMC_CHUHUOFAIL 	  12						//出货失败
#define VMC_PAYOUT 	      13						//找零
#define VMC_END 	      14						//结束交易
#define VMC_ERROR		  15                        //故障状态
#define VMC_ERRORSALE	  16                        //故障时投入钱币的状态
#define VMC_ERRORPAYOUT	  17                        //故障时找零的状态
#define VMC_ERROREND	  18                        //故障时结束交易的状态
#define VMC_WEIHU		  19                        //维护状态



char     	ChannelNum[3] = {0};//选择货道值
uint8_t	    channelInput = 0;
uint8_t	 	vmcStatus = VMC_FREE;//当前状态
uint8_t	 	vmcChangeLow = 0;//零钱是否不够找1不够找,0够找
uint8_t	    isTuibi = 0;//是否按下退币按钮,1是
uint8_t	 	vmcEorr = 0;//本机故障,1故障

uint32_t 	g_coinAmount = 0;   	 //当前投币硬币总金额
uint32_t 	g_billAmount = 0;    //当前压入纸币总金额
uint32_t 	g_holdValue = 0;    //当前暂存纸币金额
uint32_t 	g_readerAmount = 0; //当前读卡器总金额

uint16_t	vmcColumn = 0;//当前购买的商品编号
uint32_t 	vmcPrice  = 0;//当前购买的商品单价
unsigned char transMul = 0;//记录是第几次交易

uint32_t 	MoneyMaxin = 0;//单次交易最大投币上限
uint32_t 	PriceMaxin = 0;//当前最贵商品单价

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
	InitTotalLog();
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
** Function name:     	GetAmountMoney
** Descriptions:	    投币总金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t GetAmountMoney()
{	
	return g_coinAmount + g_billAmount + g_holdValue + g_readerAmount;
}

/*********************************************************************************************************
** Function name:       IsTuibiAPI
** Descriptions:        是否有按下找零器
** input parameters:    无
** output parameters:   无
** Returned value:      1按下，0没按下
*********************************************************************************************************/
uint8_t IsTuibiAPI()
{
	if(isTuibi==1)
	{
		isTuibi=0;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*********************************************************************************************************
** Function name:     	BillCoinEnable
** Descriptions:	    使能禁能纸币器硬币器
** input parameters:    billCtr控制纸币器=1,使能,2禁能,0不操作
                        coinCtr控制硬币器=1,使能,2禁能,0不操作
                        readerCtr控制读卡器=1,使能,2禁能,0不操作
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BillCoinCtr(uint8_t billCtr,uint8_t coinCtr,uint8_t readerCtr)
{
	uint32_t InValue=0;
	uint8_t  billOptBack = 0;
	uint8_t Billtype = 0;
	//1.纸币器控制
	if(VMCParam.MdbBillDeviceEAB == 0x01)
	{
		switch(billCtr)
		{
			case 1:
				Trace("\r\n AppenBill");
				BillDevProcess(&InValue,&Billtype,MBOX_BILLENABLEDEV,&billOptBack);
				break;
			case 2:
				Trace("\r\n AppdisBill");
				BillDevProcess(&InValue,&Billtype,MBOX_BILLDISABLEDEV,&billOptBack);
				break;
			default:break;	
		}
	}
	//2.硬币器控制
	if(VMCParam.MdbCoinDeviceEAB == 0x01)
	{
		switch(coinCtr)
		{
			case 1:
				Trace("\r\n AppenCoin");
				MdbCoinTypeEanbleOrDisable(1,1);
				break;
			case 2:
				Trace("\r\n AppdisCoin");
				MdbCoinTypeEanbleOrDisable(0,1);
				break;
			default:break;	
		}
	}
	//3.读卡器控制
	if(VMCParam.MdbCashlessDeviceEAB == 0x01)
	{
		switch(readerCtr)
		{
			case 1:
				//ReaderDevEnableAPI();
				break;
			case 2:
				//ReaderDevDisableAPI();
				break;
			default:break;	
		}
	}
}

/*********************************************************************************************************
** Function name:     	GetMoney
** Descriptions:	    回到空闲状态前，先清除交易的数据
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ClearDealPar()
{
	channelInput = 0;
	memset(ChannelNum,0,sizeof(ChannelNum));
}

/*********************************************************************************************************
** Function name:     	DispBusinessText
** Descriptions:	    清交易时提示语言
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CLrBusinessText()
{	
	API_LCM_ClearArea(10,13,240,14);
	vTaskDelay(2);	
}

/*********************************************************************************************************
** Function name:     	DispFreePage
** Descriptions:	    空闲页面
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispFreePage()
{
	API_LCM_ClearArea(70,6,240,8);
	vTaskDelay(2);
		
	
	if(vmcEorr==1)
	{		
		API_LCM_Printf(70,6,0,0,UIMenu.error[VMCParam.Language]);
	}
	else if(vmcChangeLow==1) 
	{
		API_LCM_Printf(70,6,0,0,UIMenu.changeempty[VMCParam.Language]);
	}
	else
	{		
		API_LCM_Printf(90,6,0,0,UIMenu.welcome[VMCParam.Language]);
	}
	
	vTaskDelay(10);
	API_RTC_Read((void *)Rtc);
	API_LCM_Printf(40,8,0,0,"%04D/%02D/%02D %02D:%02D",(((unsigned int)Rtc[5] << 8) + Rtc[6]),Rtc[4],Rtc[3],Rtc[2],Rtc[1]);
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

/*********************************************************************************************************
** Function name:     	DispChaxunPage
** Descriptions:	    输入查询货道的页面
** input parameters:    keyMode=1,表明这个按键是选货按钮按下的,
                               =2,表明这个按键是小键盘按下的,
                               =0表明没有按键按下
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispChaxunPage(uint8_t *keyValue)
{
	unsigned char layer = 0x00,channel = 0x00;
	char    *pstr;
	char	strMoney[10];

	
	//取消
	if(*keyValue == '>')
	{
		ClearDealPar();
		CLrBusinessText();
		API_LCM_Printf(10,13,0,0,UIMenu.column[VMCParam.Language],"--");
		vmcStatus = VMC_SALE;
		return;
	}		
	
	ChannelNum[channelInput++] = *keyValue; 
	API_LCM_Printf(10,13,0,0,UIMenu.column[VMCParam.Language],ChannelNum);

	if(channelInput >= 2)
	{
		layer = ChannelNum[0];
		channel = ChannelNum[1] - 0x30;
		vmcColumn = ((layer - 0x41)*8 + (channel - 1));
		Trace("%s column%ld=%d,%d,%d\r\n",ChannelNum,vmcColumn,VMCParam.GoodsChannelArray[vmcColumn],TradeParam.GoodsPrice[vmcColumn],
			TradeParam.RemainGoods[vmcColumn]);
		if(
			//货道未开启
			(VMCParam.GoodsChannelArray[vmcColumn]==0)
			//货道单价为0
		    ||(TradeParam.GoodsPrice[vmcColumn]==0)			
		)
		{
			ClearDealPar();
			CLrBusinessText();
			API_LCM_Printf(10,13,0,0,UIMenu.column[VMCParam.Language],"--");
			vmcStatus = VMC_SALE;
		}
		//货道当前存货数量
		else if(TradeParam.RemainGoods[vmcColumn]==0)	
		{
			ClearDealPar();
			CLrBusinessText();
			API_LCM_Printf(10,13,0,0,UIMenu.column[VMCParam.Language],"货道已售完");
			vmcStatus = VMC_SALE;
		}
		else
		{
			vmcPrice=(uint32_t)TradeParam.GoodsPrice[vmcColumn]*10;//以分为单位处理
			pstr = PrintfMoney(vmcPrice);
			strcpy(strMoney, pstr);
			API_LCM_Printf(10,13,0,0,UIMenu.price[VMCParam.Language],ChannelNum,strMoney);
			ClearDealPar();
			if(GetAmountMoney()>=vmcPrice)
			{
				vmcStatus = VMC_CHUHUO;
			}
			else
			{				
				vmcStatus = VMC_SALE;
			}
		}
	}
		
}
/*********************************************************************************************************
** Function name:     	DispSalePage
** Descriptions:	    投币进行交易页面
** input parameters:    haveSale是否进行过一次交易
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispSalePage()
{
	char    *pstr;
	char	strMoney[10];
	
	
	pstr = PrintfMoney(GetAmountMoney());
	strcpy(strMoney, pstr);
	API_LCM_ClearScreen();
	vTaskDelay(2);
	API_LCM_Printf(60,6,0,0,UIMenu.amount[VMCParam.Language],strMoney);
	API_LCM_DrawLine(0,12);
	API_LCM_Printf(10,13,0,0,UIMenu.column[VMCParam.Language],"");
}

/*********************************************************************************************************
** Function name:     	DispChuhuoPage
** Descriptions:	    出货
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispChuhuoPage()
{
	CLrBusinessText();
	API_LCM_Printf(10,13,0,0,UIMenu.dispense[VMCParam.Language]);
	vTaskDelay(500*3);
}

/*********************************************************************************************************
** Function name:     	DispQuhuoPage
** Descriptions:	    取货
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispQuhuoPage()
{
	CLrBusinessText();
	API_LCM_Printf(10,13,0,0,UIMenu.takecolumn[VMCParam.Language]);
	vTaskDelay(500*3);
}

/*********************************************************************************************************
** Function name:     	DispChhuoFailPage
** Descriptions:	    出货失败
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispChhuoFailPage()
{
	CLrBusinessText();
	API_LCM_Printf(10,13,0,0,UIMenu.soldout[VMCParam.Language]);
	vTaskDelay(500*3);
}


/*********************************************************************************************************
** Function name:     	DispPayoutPage
** Descriptions:	    正在找零
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispPayoutPage()
{
	char    *pstr;
	char	strMoney[10];

	CLrBusinessText();
	pstr = PrintfMoney(GetAmountMoney());
	strcpy(strMoney, pstr);
	API_LCM_Printf(10,13,0,0,UIMenu.payout[VMCParam.Language],strMoney);
}

/*********************************************************************************************************
** Function name:     	DispQuChangePage
** Descriptions:	    取零钱
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispQuChangePage()
{
	CLrBusinessText();
	API_LCM_Printf(10,13,0,0,UIMenu.takemoney[VMCParam.Language]);
	vTaskDelay(500*3);
}

/*********************************************************************************************************
** Function name:     	DispIOUPage
** Descriptions:	    找零失败
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispIOUPage(uint32_t debtMoney)
{
	char    *pstr;
	char	strMoney[10];

	CLrBusinessText();
	pstr = PrintfMoney(debtMoney);
	strcpy(strMoney, pstr);
	API_LCM_Printf(10,13,0,0,UIMenu.IOU[VMCParam.Language],strMoney);
	vTaskDelay(500*3);
}
/*********************************************************************************************************
** Function name:     	DispEndPage
** Descriptions:	    交易结束
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispEndPage()
{
	API_LCM_ClearScreen();	
	vTaskDelay(20);
	API_LCM_Printf(60,6,0,0,UIMenu.end[VMCParam.Language]);
	vTaskDelay(500*3);
}

/*********************************************************************************************************
** Function name:     	UnpdateTubeMoney
** Descriptions:	    更新可找零金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void UnpdateTubeMoney()
{
	uint32_t coinMoney=0;	
	uint8_t i;
	MdbCoinGetTubeStatus();
	for(i=0;i<16;i++)
	{
		coinMoney+=MDBCoinDevice.CoinTypePresentInTube[i]*MdbGetCoinValue(MDBCoinDevice.CoinTypeCredit[i]);
	}
	Trace("\r\n APP>>Tube=%d\r\n",coinMoney);	
	//零钱小于3元
	if(coinMoney<300)
	{
		if(vmcChangeLow==0)
		{
			vmcChangeLow=1;
			BillCoinCtr(2,2,0);
		}
	}
	else
	{
		if(vmcChangeLow==1)
		{
			vmcChangeLow=0;
			BillCoinCtr(1,1,0);
		}
	}
	vTaskDelay(10);	
}
/*********************************************************************************************************
** Function name:     	GetMoney
** Descriptions:	    得到交易金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t GetMoney()
{
	uint32_t InValue=0,billEscrow=0;
	uint8_t  billOptBack = 0;
	uint8_t Billtype = 0;

	
	//1.硬币轮询
	if(VMCParam.MdbCoinDeviceEAB == 0x01)
	{
		vTaskDelay(20);
		CoinDevProcess(&InValue,&Billtype,&billOptBack);
		if(InValue>0)
		{			
			g_coinAmount += InValue;
			Trace("\r\nAppcoin1.coin=%ld",InValue);
			LogGetMoneyAPI(InValue,1);//记录日志
			return 1;		
		}
		if(billOptBack==1)
		{
			Trace("\r\nAppcoinTuibi");
			API_BUZZER_Buzzer();
			isTuibi=1;
		}
	}
	//2.纸币轮询
	if(VMCParam.MdbBillDeviceEAB == 0x01)
	{
		//判断是否有纸币投入
		vTaskDelay(20);
		BillDevProcess(&InValue,&Billtype,0,&billOptBack);
		if(InValue>0)
		{
			Trace("\r\n APP>>nAppbill1.bill=%d\r\n",InValue);
			BillDevProcess(&billEscrow,&Billtype,MBOX_BILLESCROW,&billOptBack);
			if(billOptBack==2)
			{
				Trace("\r\n TaskEscrowSuccess=%d\r\n",InValue);		
				g_billAmount += InValue;
				LogGetMoneyAPI(InValue,2);//记录日志
				InValue = 0;
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	return 0;
}

/*********************************************************************************************************
** Function name:       ChangerMoney
** Descriptions:        找零
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t ChangerMoney(void)
{	
	uint32_t tempmoney=0,backmoney=0;
	

	Trace("\r\n AppChange=%d",GetAmountMoney());
	tempmoney=GetAmountMoney();
	//找零硬币
	if(GetAmountMoney())
	{		
		ChangePayoutProcessLevel3(GetAmountMoney(),&backmoney);	
		Trace("\r\n Appbackmoney=%d",backmoney);
	}	
	

	LogChangeAPI(backmoney);//记录日志
	//找零失败
	if(tempmoney>backmoney)
	{		
		Trace("\r\n Appchange Fail");		
		g_coinAmount = 0;
		g_billAmount = 0;
		return  tempmoney-backmoney;
	}
	//找零成功
	else
	{
		Trace("\r\n Appchange succ");
		g_coinAmount = 0;
		g_billAmount = 0;
		return 0;
	}
}

/*********************************************************************************************************
** Function name:     	SaleCostMoney
** Descriptions:	    出货后扣除金额
** input parameters:    PriceSale出货单价
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SaleCostMoney(uint32_t PriceSale)
{	
	
	//扣除单价
	if(PriceSale)
	{
		/*//向读卡器发送交易成功
		if(g_readerAmount > 0)
		{
			TraceReader("\r\n AppCHuhuoSucc");
			ReaderDevVendoutResultAPI(1);
			g_readerAmount -= PriceSale;
			PriceSale = 0;
		}		
		else 
		*/
		if(PriceSale >= g_billAmount)
		{
			PriceSale -= g_billAmount;
			g_billAmount = 0;
		}
		else
		{
			g_billAmount -= PriceSale;
			PriceSale = 0;
		}
	}
	if(PriceSale)
	{
		g_coinAmount -= PriceSale;					
	}	
}

/*********************************************************************************************************
** Function name:       IsErrorState
** Descriptions:        是否进入故障状态
** input parameters:    无
** output parameters:   无
** Returned value:      1故障设备，0正常 
*********************************************************************************************************/
uint8_t IsErrorState()
{ 
	uint8_t coinError = 0,billError = 0;
	//纸币器	
	if(VMCParam.MdbBillDeviceEAB == 0x01)
	{
		if(
			(MdbBillErr.Communicate)||(MdbBillErr.moto)||(MdbBillErr.sensor)||(MdbBillErr.romchk)
			||(MdbBillErr.jam)||(MdbBillErr.removeCash)||(MdbBillErr.cashErr)
		  )
		{
			//billError = 1;			
		}
		
	}
	Trace("\r\n BillErr=%d,%d,%d,%d,%d,%d,%d",MdbBillErr.Communicate,MdbBillErr.moto,MdbBillErr.sensor,MdbBillErr.romchk,
		MdbBillErr.jam,MdbBillErr.removeCash,MdbBillErr.cashErr);
	
	//硬币器
	if(VMCParam.MdbCoinDeviceEAB == 0x01)
	{
		if(
			(MdbCoinErr.Communicate)||(MdbCoinErr.sensor)||(MdbCoinErr.tubejam)||(MdbCoinErr.romchk)
			||(MdbCoinErr.routing)||(MdbCoinErr.jam)||(MdbCoinErr.removeTube)
		  )
		{
			coinError = 1;			
		}		
	}
	Trace("\r\n CoinErr=%d,%d,%d,%d,%d,%d,%d",MdbCoinErr.Communicate,MdbCoinErr.sensor,MdbCoinErr.tubejam,MdbCoinErr.romchk,
		MdbCoinErr.routing,MdbCoinErr.jam,MdbCoinErr.removeTube);
	
	if(coinError||billError)
	{
		vmcEorr=1;
		return 1;
	}	
	else
	{
		vmcEorr=0;
		return 0;
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
	uint8_t  billOptBack = 0;
	uint32_t InValue = 0;
	uint8_t Billtype = 0;

	uint8_t keyValue = 0;//按键值
	uint8_t moneyGet = 0;//是否已经有钱了
	uint32_t debtMoney;	
	uint8_t ChuhuoRst = 0;
	
	LPC_GPIO2->FIODIR |= 1ul <<12;
	API_LCM_ClearScreen();

	vTaskDelay(100);
	//初始化硬币器
	if(VMCParam.MdbCoinDeviceEAB == 0x00)
	{
		API_LCM_Printf(0,0,0,0,"CoinDevice is disable by system.\r\n");
	}
	else
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
	}

	//初始化纸币器
	if(VMCParam.MdbBillDeviceEAB == 0x00)
	{
		API_LCM_Printf(0,0,0,0,"BillDevice is disable by system.\r\n");
	}
	else
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
	API_SYSTEM_TimerChannelSet(4,5 * 100);	
	
	
	
	while(1)
	{
		switch(vmcStatus)
		{
			case VMC_FREE:
				//1.显示空闲页面
				if(API_SYSTEM_TimerReadChannelValue(1)==0)
				{
					API_SYSTEM_TimerChannelSet(1,5 * 100);
					DispFreePage();
				}
				//4.检测设备故障状态
				if(API_SYSTEM_TimerReadChannelValue(4)==0)
				{
					API_SYSTEM_TimerChannelSet(4,5 * 100);				
					if(IsErrorState())
					{
						API_SYSTEM_TimerChannelSet(1,0);
						BillCoinCtr(2,2,0);
						vmcStatus = VMC_ERROR;
					}
				}			
				//轮询投纸币和硬币金额	
				moneyGet = GetMoney();
				if(moneyGet == 1)
				{
					isTuibi=0;
					DispSalePage();
					Trace("\r\n App2amount=%ld",GetAmountMoney());					
					vmcStatus = VMC_SALE;
				}
				//2.轮询硬币器可找零硬币
				if(API_SYSTEM_TimerReadChannelValue(2)==0)
				{
					API_SYSTEM_TimerChannelSet(2,5 * 100);
					UnpdateTubeMoney();
				}
				//判断是否进入维护页面
				if(API_KEY_ReadKey() == 'M')
				{
					VMCParam.VMCMode = VMC_MODE_MAIN;
					break;
				}
				break;
			case VMC_CHAXUN:
				//1.轮询按键
				if(keyValue)
				{
					Trace("\r\n AppKey2=%d",keyValue);
					API_SYSTEM_TimerChannelSet(3,5 * 100);
					DispChaxunPage(&keyValue);
				}
				keyValue = API_KEY_ReadKey();
				if(keyValue)
				{					
					if((keyValue >= '1') && (keyValue <= '8'))
					{}
					//取消
					else if(keyValue=='>')
					{}
					else
					{
						keyValue=0;
					}
				}
				//轮询投纸币和硬币金额
				moneyGet = GetMoney();
				if(moneyGet == 1)
				{
					DispSalePage();
					Trace("\r\n App2amount=%ld",GetAmountMoney());	 				
				}
				//2超时退出查询页面
				if(API_SYSTEM_TimerReadChannelValue(3)==0)
				{
					keyValue = '>';
					API_SYSTEM_TimerChannelSet(3,5 * 100);
				}	
				//有按下退币按键
				if(IsTuibiAPI())
				{
					Trace("\r\n App2Return");
					BillCoinCtr(2,2,0);
					vmcStatus = VMC_PAYOUT;
				}
				break;	
			case VMC_SALE:
				//1.轮询投纸币和硬币金额
				moneyGet = GetMoney();
				if(moneyGet == 1)
				{
					DispSalePage();
					Trace("\r\n App2amount=%ld",GetAmountMoney());	 				
				}
				//2.轮询按键
				keyValue = API_KEY_ReadKey();
				if(keyValue)
				{
					//确定
					if((keyValue >= 'A') && (keyValue <= 'F'))
					{
						Trace("\r\n AppKey1=%d",keyValue);
						API_SYSTEM_TimerChannelSet(3,5 * 100);
						vmcStatus = VMC_CHAXUN;	
						CLrBusinessText();
						break;
					}
				}
				
				//有按下退币按键
				if(IsTuibiAPI())
				{
					Trace("\r\n App2Return");
					BillCoinCtr(2,2,0);
					vmcStatus = VMC_PAYOUT;
				}
				break;
			case VMC_CHUHUO:
				BillCoinCtr(2,2,0);
				vTaskDelay(500*3);				
				DispChuhuoPage();				
				//ChuhuoRst = ChannelAPIProcess(vmcColumn);	
				ChuhuoRst=1;
				if(ChuhuoRst==1)
				{
					Trace("\r\n chuhuosuc\r\n");	
					//货道存货数量递减
					if(TradeParam.RemainGoods[vmcColumn]>0)
					{
						TradeParam.RemainGoods[vmcColumn]--;
						LoadNewTradeParam();
					}
					DispQuhuoPage();	
					SaleCostMoney(vmcPrice);//扣款				
					Trace("\r\n nowmoney=%d\r\n",GetAmountMoney());						
					LogTransactionAPI(1);//记录日志
				}
				else
				{
					Trace("\r\n chuhuofail\r\n");
					DispChhuoFailPage();
					Trace("\r\n nowmoney=%d\r\n",GetAmountMoney());	
					LogTransactionAPI(0);//记录日志
				}
				vmcPrice = 0;
				vmcColumn=0;
				ClearDealPar();
				if(GetAmountMoney())
				{
					BillCoinCtr(1,1,0);	
					DispSalePage();
					vmcStatus = VMC_SALE;
				}
				else
				{
					vmcStatus = VMC_END;
				}	
				break;	
			case VMC_PAYOUT:				
				DispPayoutPage();
				debtMoney = ChangerMoney();
				if(debtMoney)
				{
					DispIOUPage(debtMoney);
				}
				else
				{
					DispQuChangePage();
				}				
				vmcStatus = VMC_END;
				break;
			case VMC_END:				
				DispEndPage();
				LogEndAPI();
				ClearDealPar();
				transMul = 0;
				vmcColumn = 0;	
				BillCoinCtr(1,1,0);				
				API_LCM_ClearScreen();
				vmcStatus = VMC_FREE;
				break;	
			case VMC_ERROR:					
				//1.显示故障页面
				if(API_SYSTEM_TimerReadChannelValue(1)==0)
				{
					API_SYSTEM_TimerChannelSet(1,5 * 100);
					DispFreePage();
				}	
				//2.检测设备是否恢复正常
				if(API_SYSTEM_TimerReadChannelValue(4)==0)
				{
					API_SYSTEM_TimerChannelSet(4,5 * 100);	
					if(!IsErrorState())
					{	
						API_SYSTEM_TimerChannelSet(1,0);
						BillCoinCtr(1,1,1);
						vmcStatus = VMC_FREE;
					}
				}
				//只是轮询
				GetMoney();
				//判断是否进入维护页面
				if(API_KEY_ReadKey() == 'M')
				{
					VMCParam.VMCMode = VMC_MODE_MAIN;
					break;
				}
				break;
		}	

		//判断是否进入维护页面
		if(VMCParam.VMCMode == VMC_MODE_MAIN)
		{
			vmcStatus = VMC_FREE;
			break;
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
