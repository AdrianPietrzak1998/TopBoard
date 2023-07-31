/*
 * ws28xx_fx.c
 *
 *  Created on: Jul 21, 2023
 *      Author: Adrian
 */

#include "ws28xx_fx.h"
#include "ws2812b.h"

#define LONG_LED_NUMBER 15
#define SHORT_LED_NUMBER 11

struct ws28xxFx ws28xxFx;
enum Mode Mode;

void ws28LedFxInit(uint16_t FxDelay, uint16_t BlinkDelay, uint8_t h, uint8_t s, uint8_t v, uint8_t Mode, uint8_t ReverseDirection)
{
	led_init();
	ws28xxFx.FxDelay = FxDelay;
	ws28xxFx.BlinkDelay = BlinkDelay;
	ws28xxFx.h = h;
	ws28xxFx.s = s;
	ws28xxFx.v = v;
	ws28xxFx.Mode = Mode;
	ws28xxFx.ReverseDirection = ReverseDirection;
	ws28xxFx.OldTick = HAL_GetTick();
}

static void ws28LedStepSet1(uint32_t nLed)
{
	  led_set_one_led_colors_hsv((nLed<4)?(NUMBER_OF_LEDS-1) - 3 + nLed:nLed-4, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/8);
	  led_set_one_led_colors_hsv((nLed<3)?(NUMBER_OF_LEDS-1) - 2 + nLed:nLed-3, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/6);
	  led_set_one_led_colors_hsv((nLed<2)?(NUMBER_OF_LEDS-1) - 1 + nLed:nLed-2, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/4);
	  led_set_one_led_colors_hsv((nLed<1)?NUMBER_OF_LEDS-1:nLed-1, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/2);
	  led_set_one_led_colors_hsv(nLed, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v);
	  led_set_one_led_colors_hsv((nLed+1>NUMBER_OF_LEDS-1)?(nLed+1) - NUMBER_OF_LEDS:nLed+1, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/2);
	  led_set_one_led_colors_hsv((nLed+2>NUMBER_OF_LEDS-1)?(nLed+2) - NUMBER_OF_LEDS:nLed+2, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/4);
	  led_set_one_led_colors_hsv((nLed+3>NUMBER_OF_LEDS-1)?(nLed+3) - NUMBER_OF_LEDS:nLed+3, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/6);
	  led_set_one_led_colors_hsv((nLed+4>NUMBER_OF_LEDS-1)?(nLed+4) - NUMBER_OF_LEDS:nLed+4, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/8);
}

static void ws28LedStepSet2(uint32_t nLed)
{
	led_set_one_led_colors_hsv(nLed, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v);
}

static void ws28LedStepSet3(uint32_t nLed)
{
	  led_set_all_led_colors_hsv(ws28xxFx.h, ws28xxFx.s, ws28xxFx.v);
	  led_set_one_led_colors_hsv((nLed<4)?(NUMBER_OF_LEDS-1) - 3 + nLed:nLed-4, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/2);
	  led_set_one_led_colors_hsv((nLed<3)?(NUMBER_OF_LEDS-1) - 2 + nLed:nLed-3, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/4);
	  led_set_one_led_colors_hsv((nLed<2)?(NUMBER_OF_LEDS-1) - 1 + nLed:nLed-2, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/6);
	  led_set_one_led_colors_hsv((nLed<1)?NUMBER_OF_LEDS-1:nLed-1, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/8);
	  led_set_one_led_colors_hsv(nLed, ws28xxFx.h, ws28xxFx.s, 0);
	  led_set_one_led_colors_hsv((nLed+1>NUMBER_OF_LEDS-1)?(nLed+1) - NUMBER_OF_LEDS:nLed+1, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/8);
	  led_set_one_led_colors_hsv((nLed+2>NUMBER_OF_LEDS-1)?(nLed+2) - NUMBER_OF_LEDS:nLed+2, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/6);
	  led_set_one_led_colors_hsv((nLed+3>NUMBER_OF_LEDS-1)?(nLed+3) - NUMBER_OF_LEDS:nLed+3, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/4);
	  led_set_one_led_colors_hsv((nLed+4>NUMBER_OF_LEDS-1)?(nLed+4) - NUMBER_OF_LEDS:nLed+4, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v/2);
}

static void ws28LedStepSet4(uint32_t nLed)
{
	led_set_all_led_colors_hsv(ws28xxFx.h, ws28xxFx.s, ws28xxFx.v);
	led_set_one_led_colors_hsv(nLed, ws28xxFx.h, ws28xxFx.s, 0);
}



void ws28LedBlink(void)
{
	if(HAL_GetTick() - ws28xxFx.OldTick >= ws28xxFx.BlinkDelay)
	{
		static uint8_t state = 0;
		if(!state)
		{
			led_set_all_led_colors_hsv(ws28xxFx.h, ws28xxFx.s, 0);
			state = 1;
		}
		else if(state)
		{
			led_set_all_led_colors_hsv(ws28xxFx.h, ws28xxFx.s, ws28xxFx.v);
			state = 0;
		}
		led_send_led_colors_to_head();
		ws28xxFx.OldTick = HAL_GetTick();
	}
}

void ws28LedWalkingMode1(void)
{
	if(HAL_GetTick() - ws28xxFx.OldTick >= ws28xxFx.FxDelay)
	{
		led_clear();
		ws28LedStepSet1(ws28xxFx.ActualLedStep);
		led_send_led_colors_to_head();
		if(!ws28xxFx.ReverseDirection)
		{
			ws28xxFx.ActualLedStep++;
			if(ws28xxFx.ActualLedStep >= NUMBER_OF_LEDS) ws28xxFx.ActualLedStep=0;
		}
		else
		{
			if(ws28xxFx.ActualLedStep != 0) ws28xxFx.ActualLedStep--;
			else ws28xxFx.ActualLedStep = NUMBER_OF_LEDS - 1;
		}
		ws28xxFx.OldTick = HAL_GetTick();
	}
}

void ws28LedWalkingMode2(void)
{
	if(HAL_GetTick() - ws28xxFx.OldTick >= ws28xxFx.FxDelay)
	{
		led_clear();
		ws28LedStepSet2(ws28xxFx.ActualLedStep);
		led_send_led_colors_to_head();
		if(!ws28xxFx.ReverseDirection)
		{
			ws28xxFx.ActualLedStep++;
			if(ws28xxFx.ActualLedStep >= NUMBER_OF_LEDS) ws28xxFx.ActualLedStep=0;
		}
		else
		{
			if(ws28xxFx.ActualLedStep != 0) ws28xxFx.ActualLedStep--;
			else ws28xxFx.ActualLedStep = NUMBER_OF_LEDS - 1;
		}
		ws28xxFx.OldTick = HAL_GetTick();
	}
}

void ws28LedWalkingMode3(void)
{
	if(HAL_GetTick() - ws28xxFx.OldTick >= ws28xxFx.FxDelay)
	{
		led_clear();
		ws28LedStepSet3(ws28xxFx.ActualLedStep);
		led_send_led_colors_to_head();
		if(!ws28xxFx.ReverseDirection)
		{
			ws28xxFx.ActualLedStep++;
			if(ws28xxFx.ActualLedStep >= NUMBER_OF_LEDS) ws28xxFx.ActualLedStep=0;
		}
		else
		{
			if(ws28xxFx.ActualLedStep != 0) ws28xxFx.ActualLedStep--;
			else ws28xxFx.ActualLedStep = NUMBER_OF_LEDS - 1;
		}
		ws28xxFx.OldTick = HAL_GetTick();
	}
}

void ws28LedWalkingMode4(void)
{
	if(HAL_GetTick() - ws28xxFx.OldTick >= ws28xxFx.FxDelay)
	{
		led_clear();
		ws28LedStepSet4(ws28xxFx.ActualLedStep);
		led_send_led_colors_to_head();
		if(!ws28xxFx.ReverseDirection)
		{
			ws28xxFx.ActualLedStep++;
			if(ws28xxFx.ActualLedStep >= NUMBER_OF_LEDS) ws28xxFx.ActualLedStep=0;
		}
		else
		{
			if(ws28xxFx.ActualLedStep != 0) ws28xxFx.ActualLedStep--;
			else ws28xxFx.ActualLedStep = NUMBER_OF_LEDS - 1;
		}
		ws28xxFx.OldTick = HAL_GetTick();
	}
}

void ws28LedWalkingMode5(void)
{
	if(HAL_GetTick() - ws28xxFx.OldTick >= ws28xxFx.BlinkDelay)
	{
		static uint8_t state = 0;
		if(!state)
		{
			for(uint8_t i=0; i<NUMBER_OF_LEDS; i++)
			{
				if(i<LONG_LED_NUMBER || (i>= LONG_LED_NUMBER + SHORT_LED_NUMBER && i<NUMBER_OF_LEDS - SHORT_LED_NUMBER))
				{
					led_set_one_led_colors_hsv(i, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v);
				}
				else
				{
					led_set_one_led_colors_hsv(i, ws28xxFx.h, ws28xxFx.s, 0);
				}
			}
			state = 1;
		}
		else
		{
			for(uint8_t i=0; i<NUMBER_OF_LEDS; i++)
			{
				if(i<LONG_LED_NUMBER || (i>= LONG_LED_NUMBER + SHORT_LED_NUMBER && i<NUMBER_OF_LEDS - SHORT_LED_NUMBER))
				{
					led_set_one_led_colors_hsv(i, ws28xxFx.h, ws28xxFx.s, 0);
				}
				else
				{
					led_set_one_led_colors_hsv(i, ws28xxFx.h, ws28xxFx.s, ws28xxFx.v);
				}
			}
			state = 0;
		}
		led_send_led_colors_to_head();
		ws28xxFx.OldTick = HAL_GetTick();
	}
}

void ws28Task(void)
{
	static uint16_t OldTickLedRefresh = 5000;

	switch (ws28xxFx.Mode) {
		case ONE_COLOR:
			if(HAL_GetTick() - OldTickLedRefresh >= 5000)
			{
				led_set_all_led_colors_hsv(ws28xxFx.h, ws28xxFx.s, ws28xxFx.v);
				led_send_led_colors_to_head();
			}
			break;
		case BLINK:
			 ws28LedBlink();
			 break;
		case WALKING_1:
			ws28LedWalkingMode1();
			break;
		case WALKING_2:
			ws28LedWalkingMode2();
			break;
		case WALKING_3:
			ws28LedWalkingMode3();
			break;
		case WALKING_4:
			ws28LedWalkingMode4();
			break;
		case WALKING_5:
			ws28LedWalkingMode5();
			break;
		default:
			break;
	}
}


