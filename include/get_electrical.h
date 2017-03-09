/*
 * get_electrical.h
 *
 *  Created on: 8. mar. 2017
 *      Author: Martin Rozej
 */

#ifndef INCLUDE_GET_ELECTRICAL_H_
#define INCLUDE_GET_ELECTRICAL_H_

#include	"define.h"
#include	"globals.h"
#include 	"math.h"
#include    "ADC_drv.h"
#include    "REF_gen.h"

#include    "PID_float.h"
#include    "DC_float.h"
#include    "DFT_float.h"
#include    "SLEW_float.h"
#include    "STAT_float.h"
#include    "ABF_float.h"
#include    "DELAY_float.h"

extern void get_electrical(void);

#endif /* INCLUDE_GET_ELECTRICAL_H_ */
