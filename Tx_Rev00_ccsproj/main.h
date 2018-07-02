/*
 * params.h
 *
 *  Created on: 1 Jul. 2018
 *      Author: ziyix
 */

#ifndef MAIN_H_
#define MAIN_H_

#define TASKSTACKSIZE   512

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config ledPinTable[] = {
    Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/*
 * Application button pin configuration table:
 *   - Buttons interrupts are configured to trigger on falling edge.
 */
PIN_Config buttonPinTable[] = {
    Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    Board_BUTTON1  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

/* function declaration  */
void IOpin_init();
void tHeartBeatLed_func(UArg arg0, UArg arg1);
void tHeartBeatLed_init();
void cbBtn_func(PIN_Handle handle, PIN_Id pinId);

#endif /* MAIN_H_ */
