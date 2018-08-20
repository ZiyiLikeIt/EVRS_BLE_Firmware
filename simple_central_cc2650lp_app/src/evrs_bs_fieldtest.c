/****************************************
 * 
 * @filename 	evrs_bs_fieldtest.c
 * 
 * @project 	simple_central_cc2650lp_app
 * 
 * @brief 		functionality of field test
 * 
 * @date 		17 Aug. 2018
 * 
 * @author		Ziyi@outlook.com.au
 *
 ****************************************/


#include "evrs_bs_fieldtest.h"



typedef enum{
	FIELD_IDLE,
	FIELD_INIT,
	FIELD_WR_RSP,
	FIELD_RD_RSP
} FieldState_t;


bool isRunning = false;
bool isInProgress = false;
uint32_t totalTestRound = 0;
uint32_t errorRound = 0;
uint8_t testData[2];
uint8_t randData;
Clock_Struct testClock;
extern Display_Handle dispHandle;
FieldState_t testState;




static void EBS_fieldClockTimeoutCB(UArg a0);
static void EBS_fieldTestfunc(uint8_t data);

void EBS_initFieldTest()
{
	isRunning = false;
	isInProgress = false;
	totalTestRound = 0;
	errorRound = 0;
	testState = FIELD_IDLE;
	Util_constructClock(&testClock, EBS_fieldClockTimeoutCB,
					100, 0, false, 0);
	return;
}

void EBS_startFieldTest()
{
	if (isRunning)
		return;
	isRunning = true;
	isInProgress = false;
	totalTestRound = 0;
	errorRound = 0;
	testState = FIELD_INIT;
	Util_startClock(&testClock);
	Display_print0(dispHandle, 6, 0, "Field test start");
	return;
}


void EBS_stopFieldTest()
{
	isRunning = false;
	Display_print2(dispHandle,8,0,"%d/%d wrong, stopped",errorRound,totalTestRound);
	return;
}


void EBS_notifyFieldTest(bool isWrite, uint8_t rsp)
{
	if(isWrite)
		testState = FIELD_WR_RSP;
	else
		testState = FIELD_RD_RSP;
	EBS_fieldTestfunc(rsp);
	return;
}


static void EBS_fieldClockTimeoutCB(UArg a0)
{
	if (isRunning) {
		if (!isInProgress) // run a new test
		{
			isInProgress = true;
			testState = FIELD_INIT;
			EBS_fieldTestfunc(0);
		}
		Util_startClock(&testClock);
	} else {
		if (!isInProgress) { // safe to exit
			Util_stopClock(&testClock);
			testState = FIELD_IDLE;
		} else { // wait another round till all work done
			Util_startClock(&testClock);
		}

	}
	return;
}

static void EBS_fieldTestfunc(uint8_t data)
{
	if (!isRunning) {
		Display_print2(dispHandle,8,0,"%d/%d wrong, stopped",errorRound,totalTestRound);
		isInProgress = false;
	}
	switch(testState)
	{
		case FIELD_INIT:
			testData[0] = 1; // data value length
			testData[1] = rand() % 0xFF; // data value
			SimpleBLECentral_enqueueMsg(EBS_FIELD_WRITE_EVT, EVRSPROFILE_DATA, testData);
			//Display_print2(dispHandle,8,0,"func testData [%d, 0x%02x]",testData[0],testData[1]);
			break;
		case FIELD_WR_RSP:
			SimpleBLECentral_enqueueMsg(EBS_FIELD_READ_EVT, EVRSPROFILE_DATA, NULL);
			break;
		case FIELD_RD_RSP:
			totalTestRound++;
			if (data != testData[1])
				errorRound++;
			isInProgress = false;
			Display_print2(dispHandle,8,0,"%d/%d wrong, running",errorRound,totalTestRound);
			break;
		default:
			break;

	}
}
