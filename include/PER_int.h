/****************************************************************
* FILE:         PER_int.h
* DESCRIPTION:  periodic interrupt header file
* AUTHOR:       Mitja Nemec
*
****************************************************************/
#ifndef     __PER_INT_H__
#define     __PER_INT_H__

#include    "F28x_Project.h"

#include    "define.h"
#include    "globals.h"

#include    "PWM_drv.h"
#include    "ADC_drv.h"
#include    "FB_bipolar.h"
#include    "PCB_util.h"
#include    "math.h"

#include    "DLOG_gen.h"
#include    "REF_gen.h"

#include    "PID_float.h"
#include    "DC_float.h"
#include    "DFT_float.h"
#include    "SLEW_float.h"
#include    "STAT_float.h"
#include    "ABF_float.h"
#include    "DELAY_float.h"
#include	"RES_float.h"
#include	"REP_float.h"

// korekcijski faktorji (meritev osciloskop)
#define		DEL_UDC_CORR_F	1.020895522

// definicije za status regulacije izhoda
enum OUT_STATE { REP=0, DFTF, RES};

/**************************************************************
* Funckija, ki pripravi vse potrebno za izvajanje
* prekinitvene rutine
**************************************************************/
extern void PER_int_setup(void);

#endif // end of __PER_INT_H__ definition
