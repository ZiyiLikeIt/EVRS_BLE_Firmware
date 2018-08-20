/****************************************
 *
 * @filename 	evrs_bs_typedefs.h
 *
 * @project 	simple_central_cc2650lp_app
 *
 * @brief 		common resource for evrs bs
 *
 * @date 		15 Aug. 2018
 *
 * @author		Ziyi@outlook.com.au
 *
 ****************************************/


#ifndef EVRS_BS_TYPEDEFS_H_
#define EVRS_BS_TYPEDEFS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "util.h"
#include <ti/mw/display/Display.h>



// RSSI read data structure
typedef struct {
	Clock_Struct *pClock; // pointer to clock struct
	uint16_t period;      // how often to read RSSI
	uint16_t connHandle;  // connection handle
} readRssi_t;


// profile identifier
typedef enum {
	EVRSPROFILE_SYSID,
	EVRSPROFILE_DEVID,
	EVRSPROFILE_DEST,
	EVRSPROFILE_DATA
} ProfileId_t;


// Simple BLE Central Task Events
#define SBC_START_DISCOVERY_EVT               0x0001
#define SBC_PAIRING_STATE_EVT                 0x0002
#define SBC_PASSCODE_NEEDED_EVT               0x0004
#define SBC_RSSI_READ_EVT                     0x0008
#define SBC_KEY_CHANGE_EVT                    0x0010
#define SBC_STATE_CHANGE_EVT                  0x0020
#define SBC_CONNECTING_TIMEOUT_EVT	      	  0x0040
#define EBS_FIELD_WRITE_EVT					  0x0080
#define EBS_FIELD_READ_EVT					  0x0081


#endif /* EVRS_BS_TYPEDEFS_H_ */
