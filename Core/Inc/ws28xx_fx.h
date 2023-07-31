/*
 * ws28xx_fx.h
 *
 *  Created on: Jul 21, 2023
 *      Author: Adrian
 */

#ifndef INC_WS28XX_FX_H_
#define INC_WS28XX_FX_H_

#include "main.h"

struct ws28xxFx{
	uint32_t OldTick;
	uint16_t FxDelay;
	uint16_t BlinkDelay;
	uint8_t h, s, v;
	uint8_t Mode;
	uint32_t ActualLedStep;
	uint8_t ReverseDirection;
};

enum Mode{
	ONE_COLOR = 0,
	BLINK,
	WALKING_1,
	WALKING_2,
	WALKING_3,
	WALKING_4,
	WALKING_5
};

extern struct ws28xxFx ws28xxFx;

void ws28LedFxInit(uint16_t FxDelay, uint16_t BlinkDelay, uint8_t h, uint8_t s, uint8_t v, uint8_t Mode, uint8_t ReverseDirection);
void ws28LedWalkingMode1(void);
void ws28LedWalkingMode2(void);
void ws28LedWalkingMode3(void);
void ws28LedWalkingMode4(void);
void ws28LedBlink(void);
void ws28Task(void);
#endif /* INC_WS28XX_FX_H_ */
