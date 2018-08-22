/******************************************************************************

 @file  simple_peripheral.c

 @brief This file contains the Simple BLE Peripheral sample application for use
 with the CC2650 Bluetooth Low Energy Protocol Stack.

 Group: WCS, BTS
 Target Device: CC2650, CC2640

 ******************************************************************************

 Copyright (c) 2013-2018, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
 its contributors may be used to endorse or promote products derived
 from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: ble_sdk_2_02_02_25
 Release Date: 2018-04-02 18:03:35
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

#include "hci_tl.h"
#include "gatt.h"
#include "linkdb.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "devinfoservice.h"
#include "evrs_gatt_profile.h"

#include "peripheral.h"
#include "gapbondmgr.h"

#include "osal_snv.h"
#include "icall_apimsg.h"

#include "util.h"

#ifdef USE_RCOSC
#include "rcosc_calibration.h"
#endif //USE_RCOSC

#include <ti/mw/display/Display.h>
#include "board_key.h"
#include "board_led.h"

#include "board.h"

#include "simple_peripheral.h"

/*********************************************************************
 * CONSTANTS
 */

// Advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// Limited discoverable mode advertises for 30.72s, and then stops
// General discoverable mode advertises indefinitely
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     16

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     400

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter
// update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// Whether to enable automatic parameter update request when a connection is
// formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         GAPROLE_LINK_PARAM_UPDATE_INITIATE_BOTH_PARAMS

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         6

// How often to perform periodic event (in msec)
#define SBP_PERIODIC_EVT_PERIOD               5000

// Task configuration
#define SBP_TASK_PRIORITY                     1

#ifndef SBP_TASK_STACK_SIZE
#define SBP_TASK_STACK_SIZE                   644
#endif

// Application state
typedef enum {
	APP_STATE_INIT, APP_STATE_IDLE, APP_STATE_ADVERT
} appStates_t;

// Internal Events for RTOS application
#define SBP_STATE_CHANGE_EVT                  0x0001
#define SBP_CHAR_CHANGE_EVT                   0x0002
#define SBP_PERIODIC_EVT                      0x0004
#define SBP_CONN_EVT_END_EVT                  0x0008
#define SBP_KEY_CHANGE_EVT                    0x0010

/*********************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.
typedef struct {
	appEvtHdr_t hdr;  // event header.
} sbpEvt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
Display_Handle dispHandle = NULL;

/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Semaphore globally used to post events to the application thread
static ICall_Semaphore sem;

// Clock instances for internal periodic events.
//static Clock_Struct periodicClock;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// events flag for internal application events.
static uint16_t events = 0;

// Task configuration
Task_Struct sbpTask;
Char sbpTaskStack[SBP_TASK_STACK_SIZE];

// Profile state and parameters
static appStates_t appState = APP_STATE_INIT;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] = {
		// complete name
		0x14,// length of this data
		GAP_ADTYPE_LOCAL_NAME_COMPLETE, 'S', 'i', 'm', 'p', 'l', 'e', 'B', 'L',
		'E', 'P', 'e', 'r', 'i', 'p', 'h', 'e', 'r', 'a', 'l',

		// connection interval range
		0x05,// length of this data
		GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE, LO_UINT16(
				DEFAULT_DESIRED_MIN_CONN_INTERVAL),   // 100ms
		HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL), LO_UINT16(
				DEFAULT_DESIRED_MAX_CONN_INTERVAL),   // 1s
		HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),

		// Tx power level
		0x02,// length of this data
		GAP_ADTYPE_POWER_LEVEL, 0       // 0dBm
		};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8_t advertData[] = {
// Flags; this sets the device to use limited discoverable
// mode (advertises for 30 seconds at a time) instead of general
// discoverable mode (advertises indefinitely)
		0x02,// length of this data
		GAP_ADTYPE_FLAGS,
		DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

		// service UUID, to notify central devices what services are included
		// in this peripheral
		0x03,// length of this data
		GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
		LO_UINT16(EVRSPROFILE_SERV_UUID), HI_UINT16(EVRSPROFILE_SERV_UUID) };

// GAP GATT Attributes
static uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "Simple BLE Peripheral";

// Globals used for ATT Response retransmission
static gattMsgEvent_t *pAttRsp = NULL;
static uint8_t rspTxRetry = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void SimpleBLEPeripheral_init(void);
static void SimpleBLEPeripheral_taskFxn(UArg a0, UArg a1);

static uint8_t SimpleBLEPeripheral_processStackMsg(ICall_Hdr *pMsg);
static uint8_t SimpleBLEPeripheral_processGATTMsg(gattMsgEvent_t *pMsg);
static void SimpleBLEPeripheral_processAppMsg(sbpEvt_t *pMsg);
static void SimpleBLEPeripheral_processStateChangeEvt(gaprole_States_t newState);
static void SimpleBLEPeripheral_processCharValueChangeEvt(uint8_t paramID);
//static void SimpleBLEPeripheral_performPeriodicTask(void);
//static void SimpleBLEPeripheral_clockHandler(UArg arg);

static void SimpleBLEPeripheral_sendAttRsp(void);
static void SimpleBLEPeripheral_freeAttRsp(uint8_t status);

static void SimpleBLEPeripheral_stateChangeCB(gaprole_States_t newState);
#ifndef FEATURE_OAD_ONCHIP
static void SimpleBLEPeripheral_charValueChangeCB(uint8_t paramID);
#endif //!FEATURE_OAD_ONCHIP
static void SimpleBLEPeripheral_enqueueMsg(uint8_t event, uint8_t state);

void SimpleBLEPeripheral_keyChangeHandler(uint8_t keys);
static void SimpleBLEPeripheral_handleKeys(uint8_t shift, uint8_t keys);

/*********************************************************************
 * EXTERN FUNCTIONS
 */
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t SimpleBLEPeripheral_gapRoleCBs = {
		SimpleBLEPeripheral_stateChangeCB     // Profile State Change Callbacks
		};

// GAP Bond Manager Callbacks
static gapBondCBs_t simpleBLEPeripheral_BondMgrCBs = {
		NULL, // Passcode callback (not used by application)
		NULL  // Pairing / Bonding state Callback (not used by application)
		};

// Simple GATT Profile Callbacks
static EVRSProfileCBs_t SimpleBLEPeripheral_EVRSProfileCBs = {
		SimpleBLEPeripheral_charValueChangeCB // Characteristic value change callback
		};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleBLEPeripheral_createTask
 *
 * @brief   Task creation function for the Simple BLE Peripheral.
 *
 * @param   None.
 *
 * @return  None.
 */
void SimpleBLEPeripheral_createTask(void) {
	Task_Params taskParams;

	// Configure task
	Task_Params_init(&taskParams);
	taskParams.stack = sbpTaskStack;
	taskParams.stackSize = SBP_TASK_STACK_SIZE;
	taskParams.priority = SBP_TASK_PRIORITY;

	Task_construct(&sbpTask, SimpleBLEPeripheral_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_init(void) {
	// ******************************************************************
	// N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
	// ******************************************************************
	// Register the current thread as an ICall dispatcher application
	// so that the application can send and receive messages.
	ICall_registerApp(&selfEntity, &sem);

#ifdef USE_RCOSC
	RCOSC_enableCalibration();
#endif // USE_RCOSC

	// Create an RTOS queue for message from profile to be sent to app.
	appMsgQueue = Util_constructQueue(&appMsg);

	Board_initKeys(SimpleBLEPeripheral_keyChangeHandler);
	Board_initLEDs();

	dispHandle = Display_open(Display_Type_UART, NULL);
	Display_print0(dispHandle, 0, 0, "\f");

	// Setup the GAP
	GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL,
	DEFAULT_CONN_PAUSE_PERIPHERAL);

	// Setup the GAP Peripheral Role Profile
	{
		// For all hardware platforms, device starts advertising upon initialization
		uint8_t initialAdvertEnable = FALSE;

		// By setting this to zero, the device will go into the waiting state after
		// being discoverable for 30.72 second, and will not being advertising again
		// until the enabler is set back to TRUE
		uint16_t advertOffTime = 0;

		uint8_t enableUpdateRequest = DEFAULT_ENABLE_UPDATE_REQUEST;
		uint16_t desiredMinInterval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
		uint16_t desiredMaxInterval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
		uint16_t desiredSlaveLatency = DEFAULT_DESIRED_SLAVE_LATENCY;
		uint16_t desiredConnTimeout = DEFAULT_DESIRED_CONN_TIMEOUT;

		// Set the GAP Role Parameters
		GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
				&initialAdvertEnable);
		GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
				&advertOffTime);

		GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData),
				scanRspData);
		GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData),
				advertData);

		GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8_t),
				&enableUpdateRequest);
		GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
				&desiredMinInterval);
		GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
				&desiredMaxInterval);
		GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t),
				&desiredSlaveLatency);
		GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t),
				&desiredConnTimeout);
	}

	// Set the GAP Characteristics
	GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

	// Set advertising interval
	{
		uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;

		GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInt);
		GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInt);
		GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInt);
		GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInt);
	}

	// Setup the GAP Bond Manager
	{
		uint32_t passkey = 0; // passkey "000000"
		uint8_t pairMode = GAPBOND_PAIRING_MODE_NO_PAIRING;
		uint8_t mitm = FALSE;
		uint8_t ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
		uint8_t bonding = FALSE;

		GAPBondMgr_SetParameter(GAPBOND_DEFAULT_PASSCODE, sizeof(uint32_t),
				&passkey);
		GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t),
				&pairMode);
		GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t),
				&mitm);
		GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t),
				&ioCap);
		GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t),
				&bonding);
	}

	// Initialize GATT attributes
	GGS_AddService(GATT_ALL_SERVICES);           // GAP
	GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT attributes
	DevInfo_AddService();                        // Device Information Service

	EVRSProfile_AddService(GATT_ALL_SERVICES); // EVRS GATT Profile

	// Setup the EVRSProfile Characteristic Values
	{
		uint8_t sysIdVal = 0xA1;
		uint8_t devIdVal = 0xA2;
		uint8_t destVal = 0xA3;
		uint8_t dataVal = 0xA4;

		EVRSProfile_SetParameter(EVRSPROFILE_SYSID, sizeof(sysIdVal),
				&sysIdVal);
		EVRSProfile_SetParameter(EVRSPROFILE_DEVID, sizeof(devIdVal),
				&devIdVal);
		EVRSProfile_SetParameter(EVRSPROFILE_DEST, sizeof(destVal),
				&destVal);
		EVRSProfile_SetParameter(EVRSPROFILE_DATA, sizeof(dataVal),
				&dataVal);
	}

	// Register callback with SimpleGATTprofile
	EVRSProfile_RegisterAppCBs(&SimpleBLEPeripheral_EVRSProfileCBs);

	// Start the Device
	VOID GAPRole_StartDevice(&SimpleBLEPeripheral_gapRoleCBs);

	// Start Bond Manager
	VOID GAPBondMgr_Register(&simpleBLEPeripheral_BondMgrCBs);

	// Register with GAP for HCI/Host messages
	GAP_RegisterForMsgs(selfEntity);

	// Register for GATT local events and ATT Responses pending for transmission
	GATT_RegisterForMsgs(selfEntity);

	HCI_LE_ReadMaxDataLenCmd();

	Display_print0(dispHandle, 0, 0, "\fBLE Peripheral test");
	Board_ledControl(BOARD_LED_ID_G, BOARD_LED_STATE_FLASH, 300);
	//Board_ledControl(BOARD_LED_ID_R, BOARD_LED_STATE_ON, 0);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_taskFxn
 *
 * @brief   Application task entry point for the Simple BLE Peripheral.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_taskFxn(UArg a0, UArg a1) {
	// Initialize application
	SimpleBLEPeripheral_init();

	// Application main loop
	for (;;)
	{
		// Waits for a signal to the semaphore associated with the calling thread.
		// Note that the semaphore associated with a thread is signaled when a
		// message is queued to the message receive queue of the thread or when
		// ICall_signal() function is called onto the semaphore.
		ICall_Errno errno = ICall_wait(ICALL_TIMEOUT_FOREVER);

		if (errno == ICALL_ERRNO_SUCCESS)
		{
			ICall_EntityID dest;
			ICall_ServiceEnum src;
			ICall_HciExtEvt *pMsg = NULL;

			if (ICall_fetchServiceMsg(&src, &dest,
					(void **) &pMsg) == ICALL_ERRNO_SUCCESS)
			{
				uint8 safeToDealloc = TRUE;

				if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
				{
					ICall_Stack_Event *pEvt = (ICall_Stack_Event *) pMsg;

					// Check for BLE stack events first
					if (pEvt->signature == 0xffff)
					{
						if (pEvt->event_flag & SBP_CONN_EVT_END_EVT)
						{
							// Try to retransmit pending ATT Response (if any)
							SimpleBLEPeripheral_sendAttRsp();
						}
					} else
					{
						// Process inter-task message
						safeToDealloc = SimpleBLEPeripheral_processStackMsg(
								(ICall_Hdr *) pMsg);
					}
				}

				if (pMsg && safeToDealloc)
				{
					ICall_freeMsg(pMsg);
				}
			}

			// If RTOS queue is not empty, process app message.
			while (!Queue_empty(appMsgQueue))
			{
				sbpEvt_t *pMsg = (sbpEvt_t *) Util_dequeueMsg(appMsgQueue);
				if (pMsg)
				{
					// Process message.
					SimpleBLEPeripheral_processAppMsg(pMsg);

					// Free the space from the message.
					ICall_free(pMsg);
				}
			}
		}

		if (events & SBP_PERIODIC_EVT)
		{
			events &= ~SBP_PERIODIC_EVT;

			//Util_startClock(&periodicClock);

			// Perform periodic application task
			//SimpleBLEPeripheral_performPeriodicTask();
		}

	}
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t SimpleBLEPeripheral_processStackMsg(ICall_Hdr *pMsg) {
	uint8_t safeToDealloc = TRUE;

	switch (pMsg->event)
	{
		case GATT_MSG_EVENT:
			// Process GATT message
			safeToDealloc = SimpleBLEPeripheral_processGATTMsg(
					(gattMsgEvent_t *) pMsg);
			break;

		case HCI_GAP_EVENT_EVENT:
		{
			// Process HCI message
			switch (pMsg->status)
			{
				case HCI_COMMAND_COMPLETE_EVENT_CODE:
					// Process HCI Command Complete Event
					break;

				case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
				{
					AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR, 0);
				}
					break;

				default:
					break;
			}
		}
			break;

		default:
			// do nothing
			break;
	}

	return (safeToDealloc);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processGATTMsg
 *
 * @brief   Process GATT messages and events.
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t SimpleBLEPeripheral_processGATTMsg(gattMsgEvent_t *pMsg) {
	// See if GATT server was unable to transmit an ATT response
	if (pMsg->hdr.status == blePending)
	{
		// No HCI buffer was available. Let's try to retransmit the response
		// on the next connection event.
		if (HCI_EXT_ConnEventNoticeCmd(pMsg->connHandle, selfEntity,
		SBP_CONN_EVT_END_EVT) == SUCCESS)
		{
			// First free any pending response
			SimpleBLEPeripheral_freeAttRsp(FAILURE);

			// Hold on to the response message for retransmission
			pAttRsp = pMsg;

			// Don't free the response message yet
			return (FALSE);
		}
	} else if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
	{
		// ATT request-response or indication-confirmation flow control is
		// violated. All subsequent ATT requests or indications will be dropped.
		// The app is informed in case it wants to drop the connection.

		// Display the opcode of the message that caused the violation.
		Display_print1(dispHandle, 0, 0, "FC Violated: %d",
				pMsg->msg.flowCtrlEvt.opcode);
	} else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
	{
		// MTU size updated
		Display_print1(dispHandle, 0, 0, "MTU Size: $d", pMsg->msg.mtuEvt.MTU);
	}

	// Free message payload. Needed only for ATT Protocol messages
	GATT_bm_free(&pMsg->msg, pMsg->method);

	// It's safe to free the incoming message
	return (TRUE);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_sendAttRsp
 *
 * @brief   Send a pending ATT response message.
 *
 * @param   none
 *
 * @return  none
 */
static void SimpleBLEPeripheral_sendAttRsp(void) {
	// See if there's a pending ATT Response to be transmitted
	if (pAttRsp != NULL)
	{
		uint8_t status;

		// Increment retransmission count
		rspTxRetry++;

		// Try to retransmit ATT response till either we're successful or
		// the ATT Client times out (after 30s) and drops the connection.
		status = GATT_SendRsp(pAttRsp->connHandle, pAttRsp->method,
				&(pAttRsp->msg));
		if ((status != blePending) && (status != MSG_BUFFER_NOT_AVAIL))
		{
			// Disable connection event end notice
			HCI_EXT_ConnEventNoticeCmd(pAttRsp->connHandle, selfEntity, 0);

			// We're done with the response message
			SimpleBLEPeripheral_freeAttRsp(status);
		} else
		{
			// Continue retrying
			Display_print1(dispHandle, 0, 0, "Rsp send retry: %d", rspTxRetry);
		}
	}
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_freeAttRsp
 *
 * @brief   Free ATT response message.
 *
 * @param   status - response transmit status
 *
 * @return  none
 */
static void SimpleBLEPeripheral_freeAttRsp(uint8_t status) {
	// See if there's a pending ATT response message
	if (pAttRsp != NULL)
	{
		// See if the response was sent out successfully
		if (status == SUCCESS)
		{
			Display_print1(dispHandle, 0, 0, "Rsp sent retry: %d", rspTxRetry);
		} else
		{
			// Free response payload
			GATT_bm_free(&pAttRsp->msg, pAttRsp->method);

			Display_print1(dispHandle, 0, 0, "Rsp retry failed: %d",
					rspTxRetry);
		}

		// Free response message
		ICall_freeMsg(pAttRsp);

		// Reset our globals
		pAttRsp = NULL;
		rspTxRetry = 0;
	}
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_processAppMsg(sbpEvt_t *pMsg) {
	switch (pMsg->hdr.event)
	{
		case SBP_STATE_CHANGE_EVT:
			SimpleBLEPeripheral_processStateChangeEvt(
					(gaprole_States_t) pMsg->hdr.state);
			break;

		case SBP_CHAR_CHANGE_EVT:
			SimpleBLEPeripheral_processCharValueChangeEvt(pMsg->hdr.state);
			break;

		case SBP_KEY_CHANGE_EVT:
			SimpleBLEPeripheral_handleKeys(0, pMsg->hdr.state);

		default:
			// Do nothing.
			break;
	}
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_stateChangeCB
 *
 * @brief   Callback from GAP Role indicating a role state change.
 *
 * @param   newState - new state
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_stateChangeCB(gaprole_States_t newState) {
	SimpleBLEPeripheral_enqueueMsg(SBP_STATE_CHANGE_EVT, newState);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processStateChangeEvt
 *
 * @brief   Process a pending GAP Role state change event.
 *
 * @param   newState - new state
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_processStateChangeEvt(gaprole_States_t newState) {
#ifdef PLUS_BROADCASTER
	static bool firstConnFlag = false;
#endif // PLUS_BROADCASTER

	switch (newState)
	{
		case GAPROLE_STARTED:
		{
			uint8_t ownAddress[B_ADDR_LEN];
			uint8_t systemId[DEVINFO_SYSTEM_ID_LEN];

			GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

			// use 6 bytes of device address for 8 bytes of system ID value
			systemId[0] = ownAddress[0];
			systemId[1] = ownAddress[1];
			systemId[2] = ownAddress[2];

			// set middle bytes to zero
			systemId[4] = 0x00;
			systemId[3] = 0x00;

			// shift three bytes up
			systemId[7] = ownAddress[5];
			systemId[6] = ownAddress[4];
			systemId[5] = ownAddress[3];

			DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN,
					systemId);

			appState = APP_STATE_IDLE;

			// Display device address
			Display_print0(dispHandle, 0, 0,
					Util_convertBdAddr2Str(ownAddress));
			Display_print0(dispHandle, 0, 0, "Initialized");
			Board_ledControl(BOARD_LED_ID_R, BOARD_LED_STATE_OFF, 0);

		}
			break;

		case GAPROLE_ADVERTISING:
			appState = APP_STATE_ADVERT;
			Display_print0(dispHandle, 0, 0, "Advertising");
			Board_ledControl(BOARD_LED_ID_R, BOARD_LED_STATE_FLASH, 100);
			break;

#ifdef PLUS_BROADCASTER
			/* After a connection is dropped a device in PLUS_BROADCASTER will continue
			 * sending non-connectable advertisements and shall sending this change of
			 * state to the application.  These are then disabled here so that sending
			 * connectable advertisements can resume.
			 */
			case GAPROLE_ADVERTISING_NONCONN:
			{
				uint8_t advertEnabled = FALSE;

				// Disable non-connectable advertising.
				GAPRole_SetParameter(GAPROLE_ADV_NONCONN_ENABLED, sizeof(uint8_t),
						&advertEnabled);

				advertEnabled = TRUE;

				// Enabled connectable advertising.
				GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
						&advertEnabled);

				// Reset flag for next connection.
				firstConnFlag = false;

				SimpleBLEPeripheral_freeAttRsp(bleNotConnected);
			}
			break;
#endif //PLUS_BROADCASTER

		case GAPROLE_CONNECTED:
		{
			linkDBInfo_t linkInfo;
			uint8_t numActive = 0;

			//Util_startClock(&periodicClock);

			numActive = linkDB_NumActive();

			// Use numActive to determine the connection handle of the last
			// connection
			if (linkDB_GetInfo(numActive - 1, &linkInfo) == SUCCESS)
			{
				Display_print1(dispHandle, 0, 0, "Num Conns: %d",
						(uint16_t )numActive);
				Display_print0(dispHandle, 0, 0,
						Util_convertBdAddr2Str(linkInfo.addr));
			} else
			{
				uint8_t peerAddress[B_ADDR_LEN];

				GAPRole_GetParameter(GAPROLE_CONN_BD_ADDR, peerAddress);

				Display_print0(dispHandle, 0, 0, "Connected");
				Display_print0(dispHandle, 0, 0,
						Util_convertBdAddr2Str(peerAddress));
			}
			Board_ledControl(BOARD_LED_ID_R, BOARD_LED_STATE_FLASH, 500);

#ifdef PLUS_BROADCASTER
			// Only turn advertising on for this state when we first connect
			// otherwise, when we go from connected_advertising back to this state
			// we will be turning advertising back on.
			if (firstConnFlag == false)
			{
				uint8_t advertEnabled = FALSE; // Turn on Advertising

				// Disable connectable advertising.
				GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
						&advertEnabled);

				// Set to true for non-connectabel advertising.
				advertEnabled = TRUE;

				// Enable non-connectable advertising.
				GAPRole_SetParameter(GAPROLE_ADV_NONCONN_ENABLED, sizeof(uint8_t),
						&advertEnabled);
				firstConnFlag = true;
			}
#endif // PLUS_BROADCASTER
		}
			break;

		case GAPROLE_CONNECTED_ADV:
			Display_print0(dispHandle, 0, 0, "Connected Advertising");
			break;

		case GAPROLE_WAITING:
			//Util_stopClock(&periodicClock);
			SimpleBLEPeripheral_freeAttRsp(bleNotConnected);

			Display_print0(dispHandle, 0, 0, "Disconnected");
			Board_ledControl(BOARD_LED_ID_R, BOARD_LED_STATE_OFF, 0);

			// Clear remaining lines
			//Display_clearLines(dispHandle, 3, 5);
			break;

		case GAPROLE_WAITING_AFTER_TIMEOUT:
			SimpleBLEPeripheral_freeAttRsp(bleNotConnected);

			Display_print0(dispHandle, 0, 0, "Timed Out");

			// Clear remaining lines
			//Display_clearLines(dispHandle, 3, 5);

#ifdef PLUS_BROADCASTER
			// Reset flag for next connection.
			firstConnFlag = false;
#endif //#ifdef (PLUS_BROADCASTER)
			break;

		case GAPROLE_ERROR:
			Display_print0(dispHandle, 0, 0, "Error");
			break;

		default:
			//Display_clearLine(dispHandle, 2);
			break;
	}

}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_charValueChangeCB
 *
 * @brief   Callback from Simple Profile indicating a characteristic
 *          value change.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_charValueChangeCB(uint8_t paramID) {
	SimpleBLEPeripheral_enqueueMsg(SBP_CHAR_CHANGE_EVT, paramID);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processCharValueChangeEvt
 *
 * @brief   Process a pending Simple Profile characteristic value change
 *          event.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_processCharValueChangeEvt(uint8_t paramID) {
	uint8_t newValue;

	switch (paramID)
	{
		case EVRSPROFILE_DEVID:
			EVRSProfile_GetParameter(EVRSPROFILE_DEVID, &newValue);

			Display_print1(dispHandle, 0, 0, "Device Id: 0x%02x",
					(uint8_t )newValue);
			break;

		case EVRSPROFILE_DEST:
			EVRSProfile_GetParameter(EVRSPROFILE_DEST, &newValue);

			Display_print1(dispHandle, 0, 0, "Destiny BS: 0x%02x",
					(uint8_t )newValue);
			break;

		case EVRSPROFILE_DATA:
			EVRSProfile_GetParameter(EVRSPROFILE_DATA, &newValue);

			Display_print1(dispHandle, 0, 0, "User Data: 0x%02x",
					(uint8_t )newValue);
			break;

		default:
			// should not reach here!
			break;
	}
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_enqueueMsg(uint8_t event, uint8_t state) {
	sbpEvt_t *pMsg;

	// Create dynamic pointer to message.
	if ((pMsg = ICall_malloc(sizeof(sbpEvt_t))))
	{
		pMsg->hdr.event = event;
		pMsg->hdr.state = state;

		// Enqueue the message.
		Util_enqueueMsg(appMsgQueue, sem, (uint8*) pMsg);
	}
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_keyChangeHandler
 *
 * @brief   Key event handler function
 *
 * @param   a0 - ignored
 *
 * @return  none
 */
void SimpleBLEPeripheral_keyChangeHandler(uint8_t keys) {
	SimpleBLEPeripheral_enqueueMsg(SBP_KEY_CHANGE_EVT, keys);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_handleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void SimpleBLEPeripheral_handleKeys(uint8_t shift, uint8_t keys) {
	//Display_print0(dispHandle, 0, 0, "handleKey() called");
	uint8_t advertEnable = FALSE;
	switch (appState)
	{
		case APP_STATE_INIT:
			break;

		case APP_STATE_IDLE:
			if (keys & KEY_RIGHT)
			{
				advertEnable = TRUE;
				GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
						&advertEnable);
				appState = APP_STATE_ADVERT;
			}
			break;
		case APP_STATE_ADVERT:
			if (keys & KEY_LEFT)
			{
				advertEnable = FALSE;
				GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
						&advertEnable);
				appState = APP_STATE_IDLE;
			}
			if (keys & KEY_RIGHT)
			{
				uint32 newValue = 0;
				EVRSProfile_GetParameter(EVRSPROFILE_DATA, &newValue);
				newValue += 1;
				EVRSProfile_SetParameter(EVRSPROFILE_DATA, sizeof(uint32), &newValue );
				appState = APP_STATE_IDLE;
			}
			break;
		default:
			break;

	}
}

/*********************************************************************
 *********************************************************************/