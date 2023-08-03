/*
 * utils.c
 *
 *  Created on: 4 cze 2022
 *      Author: Adrian
 */

#include "main.h"
#include "usart.h"
#include "string.h"
#include "utils.h"
#include "usbd_cdc_if.h"
//void UartLog(char * Message)
//{
//	HAL_UART_Transmit(&huart2, (uint8_t*) Message, strlen(Message), 20);
//
//}
void UsbLog(char * Message)
{
	CDC_Transmit_FS((uint8_t*) Message, strlen(Message));
}

void Temperature100ToString(int32_t temp, char *StringBuf)
{
	int16_t Decimal;
	uint16_t Fractial;

	Decimal = temp/100;

	if(temp>=0)
	{
		Fractial = temp - Decimal * 100;
	}
	else
	{
		Fractial = (temp * -1) - ((Decimal * -1) * 100);
	}

	sprintf(StringBuf, "%i.%.2u", Decimal, Fractial);
}
