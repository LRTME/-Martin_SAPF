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
#include	"RES_float.h"

// stevec prekinitev
extern volatile float    interrupt_cnt;

// seznam globalnih spremenljivk
extern volatile enum STATE { Initialization = 0, Startup, Standby, Enable, Working, Disable, Fault, Fault_sensed} state;

extern volatile enum MODE { Open_loop = 0, Control} mode;

//struktura z zastavicami napake
extern struct FAULT_FLAGS
{
    bool    overcurrent_IF:1;
    bool    overcurrent_IS:1;
    bool    HW_trip:1;
    bool    undervoltage_u_dc:1;
    bool    overvoltage_u_dc:1;
    bool    undervoltage_u_ac:1;
    bool    overvoltage_u_ac:1;
    bool	overvoltage_u_f:1;
    bool    cpu_overrun:1;
    bool    fault_registered:1;
} fault_flags;

// kdo proži trigger
extern volatile enum TRIGGER {Ref_cnt = 0, Napetost} trigger;

// signaliziram, da je offset kalibriran
extern volatile bool calibration_done;
extern volatile bool start_calibration;


// za zagon in delovanje
extern SLEW_float   u_dc_slew;
extern SLEW_float   u_out_slew;
extern PID_float    u_dc_reg;
extern PID_float	u_out_reg;


extern float u_dc;
extern float u_ac_rms;
extern float u_out;
extern float u_f;

#endif // end of __GLOBALS_H__ definition
