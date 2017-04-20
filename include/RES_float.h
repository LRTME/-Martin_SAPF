/****************************************************************
* FILENAME:     RES_float.h
* DESCRIPTION:  resonancni regulator
* AUTHOR:       M.R.
* START DATE:   13.04.2017
* VERSION:      1.0
*
****************************************************************/
#ifndef     __RES_FLOAT_H__
#define     __RES_FLOAT_H__

#include    "define.h"
#include    "math.h"


/* definicija konstant, ki jih potrebujemo  */

/* velikost nasega okna - st. vzorcev v periodi */
#define     RES_FLOAT_SIZE  SAMPLE_POINTS

// tip strukture
typedef struct RES_FLOAT_STRUCT
{
    float   In;                    	// Input
	float	InFreq;					// Input frequency
    float   Out;                  	// Output
    float   Y_A;                  	// clen Y_A
    float   Y_B;                  	// clen Y_B
    float   kot;
    float   sin;
    float   cos;
    int     m;   	                // buffer pointer
    float   Buffer[RES_FLOAT_SIZE];	// buffer A
} RES_float;


/*-----------------------------------------------------------------------------
Default initalizer for the RES_fixed object.
-----------------------------------------------------------------------------*/                     
#define RES_FLOAT_DEFAULTS       \
{          	                     \
    0.0,                         \
	0.0,						 \
    0.0,                         \
    0.0,                         \
    0.0,                         \
    0.0,                         \
    0.0,                         \
    0.0,                         \
    0,                     	     \
}

/*------------------------------------------------------------------------------
 RES Macro Definition for main function
------------------------------------------------------------------------------*/
#define RES_FLOAT_MACRO(v)                                  \
{                                                           \
    v.kot = ((2*PI*v.InFreq/RES_FLOAT_SIZE) * (v.m));		\
    v.sin = sin(v.kot);                                     \
    v.cos = cos(v.kot);                                     \
    v.Y_A = v.Y_A                                         	\
          + (v.In * v.cos) * v.cos;                         \
    v.Y_B = v.Y_B                                         	\
          + (v.In * v.sin) * v.sin;                         \
    v.Buffer[v.m] = v.In;                                   \
    v.Out = v.Y_A + v.Y_B;					              	\
    v.m++;                                                  \
    if (v.m == RES_FLOAT_SIZE) v.m = 0;                     \
}

/*------------------------------------------------------------------------------
 RES Macro Definition for initialization function
------------------------------------------------------------------------------*/
#define RES_FLOAT_MACRO_INIT(v)                 			\
{                                               			\
    for (v.m = 0; v.m < RES_FLOAT_SIZE; v.m++)  			\
    {                                           			\
        v.Buffer[v.m] = 0.0;                    			\
    }                                           			\
    v.m = 0;                                    			\
}


#endif // __RES_FIXED_H__
