/****************************************************************
* FILENAME:     REF_gen.c
* DESCRIPTION:  generator tazliènih oblik signala
* AUTHOR:       Mitja Nemec
* START DATE:   1.8.2016
****************************************************************/
#include    "REF_gen.h"

struct REF_GEN ref_gen =
{
        0.0,            //float   ref_amp;
        1.0,            //float   ref_freq;
        0.0,             //float   kot;
        0.5,            //float   ref_duty;
        0.0,            //float   ref_offset;
        1.0,            //float   ref_slew;
        SAMPLE_TIME,    //float   ref_samp_period;
        0.0,
        REF_Konst
};

// privatna funkcija za omejevanje naklona
void   ref_gen_slew_limit(float input, float *output, float slew_rate);

#pragma CODE_SECTION(REF_GEN_update, "ramfuncs");
void REF_GEN_update(void)
{
    // vrednost, ki jo pošljem ven
    float   ref_internal = 0.0;

    // najprej generiram kot
    ref_gen.kot = ref_gen.kot + ref_gen.freq * ref_gen.samp_period;
    if (ref_gen.kot > 1.0)
    {
        ref_gen.kot = ref_gen.kot - 1.0;
    }
    if (ref_gen.kot < 0.0)
    {
        ref_gen.kot = ref_gen.kot + 1.0;
    }

    // potem pa generiram referenèni signal glede na izbrano obliko
    switch(ref_gen.type)
    {
    case REF_Step:
        // generiram signal zaradi zašèite elektronike dodam omejitev naklona
        // najprej generiram step
        if (ref_gen.kot < ref_gen.duty)
        {
            ref_internal = ref_gen.amp + ref_gen.offset;
        }
        else
        {
            ref_internal = ref_gen.offset;
        }
        // omejim naklon
        // tudi pri èisti stopnici omejim naklon zaradi strojne opreme
        ref_gen_slew_limit(ref_internal, &ref_gen.out, REF_GEN_MAX_SLEW);

        break;
    case REF_Slew:
        // generiram signal
        if (ref_gen.kot < ref_gen.duty)
        {
            ref_internal = ref_gen.amp + ref_gen.offset;
        }
        else
        {
            ref_internal = ref_gen.offset;
        }
        // omejim naklon
        ref_gen_slew_limit(ref_internal, &ref_gen.out, ref_gen.slew);
        break;

    case REF_Konst:
        // generiram signal
        ref_gen_slew_limit(ref_gen.amp, &ref_gen.out, REF_GEN_MAX_SLEW);
        break;

    case REF_Sine:
        // generiram signal
        ref_gen.out = ref_gen.offset + ref_gen.amp * sin(2*PI*ref_gen.kot);
        break;

    default:
        ref_gen.out = 0.0;
        break;

    }
}

void   ref_gen_slew_limit(float input, float *output, float slew_rate)
{
    // ce je vhod vecji od izhoda
    if ((*output - input) < 0.0)
    {
        if ((-(*output - input)) > slew_rate * ref_gen.samp_period)
            *output = *output + slew_rate * ref_gen.samp_period;
        else
            *output = input;
    }
    // ce pa je vhod manjsi od izhoda
    if ((*output - input) > 0.0)
    {
        if ((*output - input) > slew_rate * ref_gen.samp_period)
            *output = *output - slew_rate * ref_gen.samp_period;
        else
            *output = input;
    }
}
