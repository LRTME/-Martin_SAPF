/****************************************************************
* FILENAME:     REP_float.h
* DESCRIPTION:  declarations of Initialization & Support Functions.
* AUTHOR:       Mitja Nemec
* START DATE:   16.1.2009
****************************************************************/
#ifndef __REP_FLOAT_H__
#define __REP_FLOAT_H__

#include    "define.h"

#define     REP_SAMPLE_SIZE     (SAMPLE_FREQ / GRID_FREQ)

typedef struct REP_FLOAT_STRUCT
{
    float   in;
    float   out;
    float   gain;
    float   w0;
    float   w1;
    float   w2;
    float   OutMax;
    float   OutMin;
    int     delay_komp;
    int     index;
    float   rep_buf[REP_SAMPLE_SIZE] ;
} REP_float;

/*-----------------------------------------------------------------------------
Default initalizer for the REP_float object.
-----------------------------------------------------------------------------*/                     
#define REP_FLOAT_DEFAULTS  \
{           \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0,      \
    0,      \
}

/*------------------------------------------------------------------------------
 REP function Definition
------------------------------------------------------------------------------*/
extern void REP_float_calc(REP_float *v);

extern void REP_float_zero(REP_float *v);

extern void REP_float_init(REP_float *v);

#endif

