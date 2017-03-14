/****************************************************************
* FILENAME:     globals.c
* DESCRIPTION:  global variables
* AUTHOR:       Mitja Nemec
*
****************************************************************/
#include "globals.h"

// stevec prekinitev
volatile float   interrupt_cnt = 0;

// spremenljivka stanja
volatile enum STATE state = Initialization;

volatile enum MODE mode = Control;

//struktura z zastavicami napake
struct FAULT_FLAGS fault_flags =
{
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE
};


// kdo proži trigger
extern volatile enum TRIGGER trigger = Ref_cnt;

// signaliziram, da je offset kalibriran
volatile bool calibration_done = FALSE;
volatile bool start_calibration = TRUE;
