/*
 * parser_simple.h
 *
 *  Created on: 4 cze 2022
 *      Author: Adrian
 */

#ifndef INC_PARSER_COMPLEX_H_
#define INC_PARSER_COMPLEX_H_

#define ENDLINE '^'

extern uint8_t FanSpeed;

/*
 * H_LED (H_VLAUE)
 * S_LED (S_VALUE)
 * V_LED (V_VALUE)
 * MODE_LED (MODE NUMBER)
 * LED_DELAY (FX_DELAY, BLINK_DELAY)
 * REV_DIR_LED (ON - 1 OFF - 0)
 * GAMMA_CORECTION (ON - 1 OFF - 0)
 */

typedef enum{
	H_LED = 0,
	S_LED,
	V_LED,
	MODE_LED,
	LED_DELAY,
	REV_DIR_LED,
	GAMMA_CORECTION,
	BOOTLOADER_JUMP
}ReceivedCommand_t;

typedef void(*CommandFun_t)();

const struct Command{
	ReceivedCommand_t CommandID;
	CommandFun_t Action;
	uint8_t CommandArgQ;
};
//
// extern declaration received function
//
extern void SetLedH(uint8_t Val);
extern void SetLedS(uint8_t Val);
extern void SetLedV(uint8_t Val);
extern void SetLedMode(uint8_t Mode);
extern void SetLedDelay(uint16_t Fx, uint16_t Blink);
extern void SetLedRevDir(uint8_t Rev);
extern void SetLedGamma(uint8_t Switch);
extern void BootloaderJump(void);

//
//
//


void Parser_TakeLine(RingBuffer_t *Buff, uint8_t *Destination);
void Parser_parse(uint8_t * DataToParse);

extern void UsbBuffWrite(char * Message);

#endif /* INC_PARSER_COMPLEX_H_ */
