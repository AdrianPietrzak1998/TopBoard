/*
 * led_blink.c
 *
 *  Created on: Jul 5, 2023
 *      Author: Adrian
 */

#include "main.h"
#include "led_blink.h"

void LedBlinkInit(blink_t *BlinkLed, GPIO_TypeDef *GpioPort, uint16_t GpioPin, uint16_t LightTime)
{
	BlinkLed -> GpioPort = GpioPort;
	BlinkLed -> GpioPin  = GpioPin;
	BlinkLed -> LightTime = LightTime;
}

void LedBlinkOne(blink_t *BlinkLed)
{
	BlinkLed -> LastTick = HAL_GetTick();
	HAL_GPIO_WritePin(BlinkLed->GpioPort, BlinkLed->GpioPin, SET);
}

void LedBlinkTask(blink_t *BlinkLed)
{
	if(HAL_GetTick() - BlinkLed->LastTick >= BlinkLed->LightTime)
	{
		HAL_GPIO_WritePin(BlinkLed->GpioPort, BlinkLed->GpioPin, RESET);
	}
}

