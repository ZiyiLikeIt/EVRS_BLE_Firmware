/****************************************
 * 
 * @filename 	evrs_bs_fieldtest.h
 * 
 * @project 	simple_central_cc2650lp_app
 * 
 * @brief 		external called function definitions
 * 
 * @date 		17 Aug. 2018
 * 
 * @author		Ziyi@outlook.com.au
 *
 ****************************************/
#ifndef EVRS_BS_FIELDTEST_H_
#define EVRS_BS_FIELDTEST_H_

// #include "util.h"
#include "evrs_bs_typedefs.h"
#include "simple_central.h"

extern void EBS_initFieldTest();
extern void EBS_startFieldTest();
extern void EBS_stopFieldTest();
extern void EBS_notifyFieldTest(bool isWrite, uint8_t rsp);

#endif
