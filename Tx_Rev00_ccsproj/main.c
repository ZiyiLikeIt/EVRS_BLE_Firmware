/*
 * main.c
 *
 *  Created on: 1 Jul. 2018
 *      Author: ziyix
 */


/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
// #include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

/* Board Header files */
#include "Board.h"
#include "main.h"
#include "smartrf_settings.h"


Task_Struct tHeartBeatLed_struct;
Char tHeartBeatLed_stack[TASKSTACKSIZE];

/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State ledPinState;
static PIN_Handle buttonPinHandle;
static PIN_State buttonPinState;


/* struct for task parameters */
Task_Params tHeartBeatLed_params;


/*
 * ========= IO pin initial =======
 *
 */
void IOpin_init()
{
    /* Open LED pins */
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);  // open pin
    if(!ledPinHandle) {  //open failed
        System_abort("Error initializing board LED pins\n");
    }

    /* Open button pin */
    buttonPinHandle = PIN_open(&buttonPinState, buttonPinTable);
    if(!buttonPinHandle) { //open failed
        System_abort("Error initializing button pins\n");
    }

    /* Setup callback for button pins */
    if (PIN_registerIntCb(buttonPinHandle, &cbBtn_func) != 0) {
        System_abort("Error registering button callback function");
    }
}
/*
 *  ======== heartBeatLed ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
void tHeartBeatLed_func(UArg arg0, UArg arg1)
{
    while (true) {
        Task_sleep((UInt)arg0);
        PIN_setOutputValue(ledPinHandle, Board_LED0,
                           !PIN_getOutputValue(Board_LED0));
    }
}

void tHeartBeatLed_init()
{
    /* Construct heartBeat Task  thread */
    Task_Params_init(&tHeartBeatLed_params);  //initial task params struct
    tHeartBeatLed_params.arg0 = 200000 / Clock_tickPeriod;
    tHeartBeatLed_params.stackSize = TASKSTACKSIZE;
    tHeartBeatLed_params.stack = &tHeartBeatLed_stack;

    Task_construct(&tHeartBeatLed_struct, (Task_FuncPtr)tHeartBeatLed_func, &tHeartBeatLed_params, NULL);  //start task
    return;
}
/*
 * ======= button function ======
 *
 */
void cbBtn_func(PIN_Handle handle, PIN_Id pinId)
{
    /* Debounce logic, only toggle if the button is still pushed (low) */
    CPUdelay(8000*50);

    if (!PIN_getInputValue(pinId)) {
        /* Toggle LED based on the button pressed */
        switch (pinId) {
        case Board_BUTTON0:
            System_printf("Button 0 pressed\n");
            System_flush();
            break;
        case Board_BUTTON1:
            System_printf("Button 1 pressed\n");
            System_flush();
            break;
        default:
            break;
        }
    }
}


/*
 *  ======== main ========
 */
int main(void)
{

    /* Call board init functions */
    Board_initGeneral();
    // Board_initI2C();
    // Board_initSPI();
    // Board_initUART();
    // Board_initWatchdog();

    IOpin_init();

    tHeartBeatLed_init();

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */

    BIOS_start();

    return (0);
}


