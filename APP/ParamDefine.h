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
#ifndef _PARADEF_H
#define _PARADEF_H
/********************************************************************************************************
** @Define name:       APP Debug
********************************************************************************************************/
#define  APP_DBG_MDB_BILL
#define  APP_DBG_MDB_COIN
#define  APP_DBG_FRAM
/********************************************************************************************************
** @Define name:       Vending Machine Parameter Define
********************************************************************************************************/
#define SOFTWAREVERSION		" V1.00"
#define MAX_MOTOR_ROW		6
#define MAX_MOTOR_COL		8
#define MAXARRAY			(MAX_MOTOR_ROW * MAX_MOTOR_COL)
#define VMC_MODE_VEND		0x01
#define VMC_MODE_MAIN		0x02
#define VMC_FRAM_ADDR		0xA0	//FRAM address
#define VMC_PARA_PAGE_S		0		//VMCParam start page
#define VMC_TDPA_PAGE_S		256		//TradeParam start page
#define VMC_TLOG_PAGE_S		512		//TotalTradeLog start page
#define VMC_SLOG_PAGE_S		1024	//TotalTradeLog start page
#define VMC_SLOG_PAGE_E		4096	//TotalTradeLog End page
/********************************************************************************************************
** @Define name:       VMCParam Parameter Define
********************************************************************************************************/
typedef struct
{
	unsigned char GoodsChannelArray[MAXARRAY];
	unsigned char GoodsMaxCapacity[MAXARRAY];
	unsigned char VMCID[7];
	unsigned char Password[6];
	unsigned char Language;
	unsigned char MdbBillDeviceEAB;
	unsigned char MdbCoinDeviceEAB;
	unsigned char MdbCashlessDeviceEAB;
	unsigned char Rtc[8];
	unsigned char VMCMode;
} VMCPARAM;
extern volatile VMCPARAM VMCParam;
/********************************************************************************************************
** @Define name:       Trade Parameter Define
********************************************************************************************************/
typedef struct
{
	unsigned short GoodsPrice[MAXARRAY];
	unsigned char  RemainGoods[MAXARRAY];
	unsigned short TypeBillEscrow;//暂存通道
	unsigned short TypeBillEnable;//纸币使能
	unsigned short TypeCoinEnable;//硬币使能
	unsigned char  ServiceWhenNoChanger;
	unsigned char  ServiceWhenBillStackerIsFull;
} TRADEPARAM;
extern volatile TRADEPARAM TradeParam;
typedef struct
{
	unsigned int TotalCoinPayIn;
	unsigned int TotalCoinPayout;
	unsigned int TotalBillPayIn;
	unsigned int TotalBillPayOut;
	unsigned int TotalCashlessPayIn;
	unsigned int TotalVendNumber;
	unsigned int TotalSuccesNumber;
	unsigned int TotalErrorNumber;
	unsigned int SingleTradeLogPointer;
} TOTALTRADELOG;
extern volatile TOTALTRADELOG TotalTradeLog;
typedef struct
{
	unsigned char Time[7];
	unsigned char Err;
	unsigned int  CoinPayIn;
	unsigned int  BillPayIn;
	unsigned int  CoinPayout;
	unsigned char Channel;
} SINGLETRADELOG;
extern volatile SINGLETRADELOG SingleTradeLog;
/********************************************************************************************************
** @Define name:       Error Code Parameter Define
********************************************************************************************************/
typedef struct
{
	unsigned char FramErr;
	unsigned char TradeParamErr;
	unsigned char MdbErr;
	unsigned char MotorErr;
} SYSTEMERROR;
extern volatile SYSTEMERROR SysError;
/********************************************************************************************************
** @Define name:       MDB Device Parameter Define
********************************************************************************************************/
typedef struct
{
	unsigned char Oneline;
	unsigned char ManufacturerCode[3];
	unsigned char Level;
	unsigned char CurrencyCode[2];
	unsigned char ScalingFactor;
	unsigned char Decimal;
	unsigned char CoinTypeRouting[2];
	unsigned char CoinTypeCredit[16];
	unsigned char CoinTypeTubeStatus[16];
	unsigned char CoinTypePresentInTube[16];
	unsigned char Feature[4];
} MDBCOINACCEPTORCHANGER;
extern volatile MDBCOINACCEPTORCHANGER MDBCoinDevice;
typedef struct
{
	unsigned char Oneline;
	unsigned char ManufacturerCode[3];
	unsigned char Level;
	unsigned char CurrencyCode[2];
	unsigned int  ScalingFactor;
	unsigned char Decimal;
	unsigned int  StackerCapacity;
	unsigned char BillSecurityLevel[2];
	unsigned char Escrow;
	unsigned char BillTypeEscrowEnable[15];
	unsigned int  BillTypeCredit[15];
	unsigned char BillRecycSupport;
	unsigned char Level2Feature[4];
	unsigned char StackerStatus;
	unsigned int  BillInStacker;
} MDBBILLVALIDATORRECYCLER;
extern volatile MDBBILLVALIDATORRECYCLER MDBBillDevice;
/********************************************************************************************************
** @Define name:       RTC Parameter Define
********************************************************************************************************/
extern volatile unsigned char Rtc[7];
/********************************************************************************************************
** @Define name:       CrcCheck Fuction
********************************************************************************************************/
extern void InitSysErrCode(void);
extern void InitVmcParam(void);
extern void LoadDefaultVmcParam(void);
extern void LoadNewVmcParam(void);
extern void InitTradeParam(void);
extern void LoadNewTradeParam(void);
extern void InitTotalLog(void);
extern void LoadNewTotalLog(void);
extern void InitSingleLog(void);
extern void LoadNewSingleLog(void);
extern unsigned short CrcCheck(unsigned char *data,unsigned int len);

#endif
/**************************************End Of File*******************************************************/
