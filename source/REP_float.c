/****************************************************************
* FILENAME:     REP_float.h
* DESCRIPTION:  declarations of Initialization & Support Functions.
* AUTHOR:       Mitja Nemec
* START DATE:   16.1.2009
****************************************************************/
#include "REP_float.h"

/*------------------------------------------------------------------------------
 PID function Definition
------------------------------------------------------------------------------*/
#pragma CODE_SECTION(REP_float_calc, "ramfuncs");
void REP_float_calc(REP_float *v)
{
    int index_temp;

    // osnovni repetitivni del
    v->rep_buf[v->index] = v->gain * v->in
                       + v->rep_buf[v->index] * v->w0;

    // prvi sosednji vzorec (za glajenje)
    if (v->w1 != 0.0)
    {
        index_temp = v->index + 1;
        if (index_temp >= SAMPLE_FREQ / 50)
        {
            index_temp = index_temp - REP_SAMPLE_SIZE;
        }
        v->rep_buf[v->index] = v->rep_buf[v->index]
                             + v->rep_buf[index_temp] * v->w1;

        index_temp = v->index - 1;
        if (index_temp < 0)
        {
            index_temp = index_temp + REP_SAMPLE_SIZE;
        }
        v->rep_buf[v->index] = v->rep_buf[v->index]
                             + v->rep_buf[index_temp] * v->w1;
    }

    // drugi sosednji vzorec (za glajenje)
    if (v->w2 != 0.0)
    {
        index_temp = v->index + 2;
        if (index_temp >= SAMPLE_FREQ / 50)
        {
            index_temp = index_temp - REP_SAMPLE_SIZE;
        }
        v->rep_buf[v->index] = v->rep_buf[v->index]
                             + v->rep_buf[index_temp] * v->w2;

        index_temp = v->index - 2;
        if (index_temp < 0)
        {
            index_temp = index_temp + REP_SAMPLE_SIZE;
        }
        v->rep_buf[v->index] = v->rep_buf[v->index]
                             + v->rep_buf[index_temp] * v->w2;
    }

    // saturacija
    v->rep_buf[v->index] = (v->rep_buf[v->index] > v->OutMax) ? v->OutMax : v->rep_buf[v->index];
    v->rep_buf[v->index] = (v->rep_buf[v->index] < v->OutMin) ? v->OutMin : v->rep_buf[v->index];

    // izhod zamaknjem
    index_temp = v->index - v->delay_komp;
    if (index_temp >= REP_SAMPLE_SIZE)
    {
        index_temp = index_temp - REP_SAMPLE_SIZE;
    }
    if (index_temp < 0)
    {
        index_temp = index_temp + REP_SAMPLE_SIZE;
    }
    v->out = v->rep_buf[index_temp];

    // pripravim za naslednjo tocko
    v->index = v->index + 1;
    if (v->index >= REP_SAMPLE_SIZE)
    {
        v->index = v->index - REP_SAMPLE_SIZE;
    }
    
    
}

void REP_float_zero(REP_float *v)
{
    v->rep_buf[v->index] = 0.0;
}


void REP_float_init(REP_float *v)
{
    for (v->index = 0; v->index < REP_SAMPLE_SIZE; v->index = v->index + 1)
    {
        v->rep_buf[v->index] = 0.0;
    }
}
