/****************************************************************
* FILENAME:     globals.h
* DESCRIPTION:  global variables header file
* AUTHOR:       Mitja Nemec
*
****************************************************************/
#ifndef     __GLOBALS_H__
#define     __GLOBALS_H__

#include    "F28x_Project.h"

#include    "define.h"
#include    "SLEW_float.h"
#include    "PID_float.h"

// stevec prekinitev
extern volatile float    interrupt_cnt;

// kdo proži trigger
extern volatile enum TRIGGER {Ref_cnt = 0, Napetost} trigger;

// signaliziram, da je offset kalibriran
extern volatile bool calibration_done;
extern volatile bool start_calibration;


// za zagon in delovanje
extern SLEW_float   DEL_UDC_slew;
extern SLEW_float   u_out_slew;
extern PID_float    u_out_reg;
extern PID_float    DEL_UDC_reg;


extern float DEL_UDC;
extern float u_ac_rms;
extern float u_out;
extern float u_f;

#endif // end of __GLOBALS_H__ definition
