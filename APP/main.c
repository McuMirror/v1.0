/****************************************Copyright (c)***************************************************
 **                      JUNPENG Easivend Co.,Ltd.
 **                               http://www.easivend.com
 **-------------------------------------File Info--------------------------------------------------------
 ** @Filename:			main.c
 ** @brief:				main  
 ** @CreatedBy:			sunway 
 ** @CreatedDate:		2016-07-01
 ** @Version:			V1.0      
 *******************************************************************************************************/
#include "..\CONFIG.H"
/********************************************************************************************************
 ** @FunctionName	main
 ** @brief 			Config CPU Clock and PIN,Create Tasks and start FreeRTOS
 ** @param  		none
 ** @return 		none
 *******************************************************************************************************/
int main(void)
{
    xTaskCreate(VendService,"VendService",2048,(void *)NULL,tskIDLE_PRIORITY+1,NULL);
	xTaskCreate(Maintenance,"Maintenance",1048,(void *)NULL,tskIDLE_PRIORITY+2,NULL);
	vTaskStartScheduler();
}
/**************************************End Of File*******************************************************/
