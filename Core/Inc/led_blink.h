/*
 * led_blink.h
 *
 *  Created on: Jul 5, 2023
 *      Author: Adrian
 */

#ifndef INC_LED_BLINK_H_
#define INC_LED_BLINK_H_

typedef struct
{
	uint32_t 	  LastTick;
	GPIO_TypeDef *GpioPort;
	uint16_t      GpioPin;
	uint16_t 	  LightTime;
}blink_t;

void LedBlinkInit(blink_t *BlinkLed, GPIO_TypeDef *GpioPort, uint16_t GpioPin, uint16_t LightTime);
void LedBlinkOne(blink_t *BlinkLed);
void LedBlinkTask(blink_t *BlinkLed);

#endif /* INC_LED_BLINK_H_ */
