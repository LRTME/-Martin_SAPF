/****************************************************************
* FILENAME:     REF_gen.h
* DESCRIPTION:  generator tazliènih oblik signala
* AUTHOR:       Mitja Nemec
* START DATE:   1.8.2016
****************************************************************/

#ifndef __INCLUDE_REF_GEN__
#define __INCLUDE_REF_GEN__

#include    "define.h"
#include    "math.h"

// maksimalna strmina (uporabljeno za konstantne in skoène spremembe)
#define REF_GEN_MAX_SLEW    10000.0

// function prototype
void REF_GEN_update(void);

// oblika generiranega signala
enum REF_GEN_TYPE { REF_Konst, REF_Step, REF_Slew, REF_Sine};

// podatkovna struktura
struct REF_GEN
{
    float   amp;
    float   freq;
    float   kot;
    float   duty;
    float   offset;
    float   slew;
    float   samp_period;
    float   out;
    enum REF_GEN_TYPE type;
};

extern struct REF_GEN ref_gen;



#endif /* INCLUDE_REF_GEN_H_ */
