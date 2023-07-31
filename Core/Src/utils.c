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
