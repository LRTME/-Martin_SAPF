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

// stevec prekinitev
extern volatile float    interrupt_cnt;

// kdo proži trigger
extern volatile enum TRIGGER {Ref_cnt = 0, Napetost} trigger;

#endif // end of __GLOBALS_H__ definition
