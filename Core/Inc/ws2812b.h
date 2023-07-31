/*
 * ws2812b.h
 *
 *  Created on: Jul 18, 2023
 *      Author: Adrian
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include "main.h"

#define RGB_LED_BUFFER_SIZE		24
#define	NUMBER_OF_LEDS			52
#define HEAD_LEDS_BUFFER_SIZE	(NUMBER_OF_LEDS*RGB_LED_BUFFER_SIZE + 1)

#define GAMMA_CORRECTION

#define H_RED 0
#define H_GREEN 85
#define H_BLUE 171

extern uint8_t led_gamma_correction_enebled;

struct __attribute__((packed)) rgb_led
{
	uint8_t green[8];
	uint8_t red[8];
	uint8_t blue[8];
};

union rgb_led_buffer
{
    uint8_t bytes[HEAD_LEDS_BUFFER_SIZE];

    struct __attribute__((packed))
    {
		struct rgb_led leds[NUMBER_OF_LEDS];
		uint8_t stop;
    };
};

void led_init(void);
void led_set_all_led_colors(uint8_t green, uint8_t red, uint8_t blue);
void led_set_all_led_colors_hsv(uint8_t h, uint8_t s, uint8_t v);
void led_set_one_led_colors(uint32_t led_number, uint8_t green, uint8_t red, uint8_t blue);
void led_set_one_led_colors_hsv(uint32_t led_number, uint8_t h, uint8_t s, uint8_t v);
void led_set_colors_data(struct rgb_led *led, uint8_t green, uint8_t red, uint8_t blue);
void led_convert_color_data_to_ws2812b_format(uint8_t color_data, uint8_t *buffer);
void led_clear(void);
void led_send_led_colors_to_head(void);


#endif /* INC_WS2812B_H_ */
