/****************************************
 *
 * @filename 	evrs_bs_main.h
 *
 * @project 	evrs_bs_cc2650lp_app
 *
 * @brief 		external typedefs, definition and variables
 *
 * @date 		22 Aug. 2018
 *
 * @author		Ziyi@outlook.com.au
 *
 ****************************************/


#ifndef SIMPLEBLECENTRAL_H
#define SIMPLEBLECENTRAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */

// GATT Params
// Simple Profile Service UUID
#define EVRSPROFILE_SERV_UUID 			0xAFF0
#define EVRSPROFILE_SYSID_UUID         	0xAFF2
#define EVRSPROFILE_DEVID_UUID      	0xAFF4
#define EVRSPROFILE_DEST_UUID        	0xAFF8
#define EVRSPROFILE_DATA_UUID         	0xAFFE

#define EVRSPROFILE_SYSID				0x00  // RW uint8
#define EVRSPROFILE_DEVID				0x01  // RW uint8
#define EVRSPROFILE_DEST				0x02  // RW uint8
#define EVRSPROFILE_DATA				0x03  // RW uint8

// Max number of connections
#define MAX_NUM_BLE_CONNS		6

/*********************************************************************
 * FUNCTIONS
 */
/*
 * Task creation function for the Simple BLE Central.
 */
extern void EBS_createTask(void);
extern uint8_t EBS_enqueueMsg(uint8_t event, uint8_t status,
		uint8_t *pData);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEBLECENTRAL_H */
