/*
 * get_electrical.c
 *
 *  Created on: 8. mar. 2017
 *      Author: Martin Rozej
 */
#include	"get_electrical.h"

// tokovi
float   IS = 0.0;
float   IF = 0.0;

float   IS_offset = 2048;
float   IF_offset = 2048;

float   IS_gain = (15.0 / 0.625 ) * (7.5 / 6.2) * (3.3 / 4096);
float   IF_gain = (25.0 / 0.625 ) * (7.5 / 6.2) * (3.3 / 4096);

long    current_offset_counter = 0;

// napetosti
float   u_ac = 0.0;

float   DEL_UDC = 0.0;
float   u_f = 0.0;
float   u_out = 0.0;

float   u_ac_offset = 2048.0;
float   DEL_UDC_offset = 2048.0;
float   u_f_offset = 2048.0;
float   u_out_offset = 2048.0;

float   u_ac_gain = ((1000 + 0.47) / (5 * 0.47)) * U_AC_CORR_F * (3.3 / 4096);
float   DEL_UDC_gain = ((36 + 1.8) / (5 * 1.8)) * (3.3 / 4096);
float   u_f_gain = ((36 + 1.8) / (5 * 1.8)) * (3.3 / 4096);
float   u_out_gain = ((1000 + 0.47) / (5 * 0.47)) * U_OUT_CORR_F * (3.3 / 4096);

// NTC
float beta_NTC = 3988;
float R_NTC25 = 5000;
float T_NTC25 = 298;
float V_NTC = 0.0;
float R_NTC = 0.0;
float T_NTC = 0.0;

// filtrirana napetost DC linka
DC_float    napetost_dc_f = DC_FLOAT_DEFAULTS;
float   napetost_dc_filtered = 0.0;

// prvi harmonik in RMS vhodne omrežne napetosti (u_ac)
DFT_float   u_ac_dft = DFT_FLOAT_DEFAULTS;
float   u_ac_rms = 0.0;
float   u_ac_form = 0.0;

// prvi harmonik in RMS izhodne napetosti (u_out)
DFT_float   u_out_dft = DFT_FLOAT_DEFAULTS;
float   u_out_rms = 0.0;
float   u_out_form = 0.0;

// regulacija napetosti enosmernega tokokroga
PID_float   DEL_UDC_reg = PID_FLOAT_DEFAULTS;
SLEW_float  DEL_UDC_slew = SLEW_FLOAT_DEFAULTS;

// regulacija omreznega toka
PID_float   IS_reg = PID_FLOAT_DEFAULTS;

// regulacija izhodne napetosti
PID_float   u_out_reg = PID_FLOAT_DEFAULTS;
SLEW_float  u_out_slew = SLEW_FLOAT_DEFAULTS;

// sinhronizacija na omrežje
float       sync_base_freq = SWITCH_FREQ;
PID_float   sync_reg    = PID_FLOAT_DEFAULTS;
float       sync_switch_freq = SWITCH_FREQ;
float       sync_grid_freq = ((SWITCH_FREQ/SAMPLING_RATIO)/SAMPLE_POINTS);
bool        sync_use = TRUE;

// samo za statistiko meritev
STAT_float  statistika = STAT_FLOAT_DEFAULTS;

// za oceno bremenskega toka
ABF_float   i_cap_abf = ABF_FLOAT_DEFAULTS;
float       IF_abf = 0.0;

// za oceno DC-link toka
ABF_float   i_cap_dc = ABF_FLOAT_DEFAULTS;
float       tok_dc_abf = 0.0;

// za zakasnitev omreznega toka
DELAY_float i_grid_delay = DELAY_FLOAT_DEFAULTS;

// filtriranje izhoda ocene
DC_float    i_dc_f = DC_FLOAT_DEFAULTS;

// filtriranje meritve
DC_float    IF_f = DC_FLOAT_DEFAULTS;

// izbira ocene izhodnega toka
volatile enum   {Meas_out = 0, ABF_out, KF_out, None_out } IF_source = Meas_out;

// izbira ocene dc toka
volatile enum   {Meas_dc = 0, ABF_dc, KF_dc, None_dc, Power_out } tok_dc_source = ABF_dc;

// izhodna moc
float   power_out = 0.0;

// temperatura hladilnika
float   temperatura = 0.0;

// meritev temperature
float NTC_temp(void);

#pragma CODE_SECTION(get_electrical, "ramfuncs");
void get_electrical(void)
{
    static float   IS_offset_calib = 0;
    static float   IF_offset_calib = 0.0;
    static float   u_ac_offset_calib = 0.0;
    static float   DEL_UDC_offset_calib = 0.0;
    static float   u_f_offset_calib = 0.0;
    static float   u_out_offset_calib = 0.0;

    // pocakam da ADC konca s pretvorbo
    ADC_wait();
    // poberem vrednosti iz AD pretvornika

    // kalibracija preostalega toka
    if (   (start_calibration == TRUE)
        && (calibration_done == FALSE))
    {
        // akumuliram offset
        IS_offset_calib = IS_offset_calib + IS_adc;
        IF_offset_calib = IF_offset_calib + IF_adc;
        u_ac_offset_calib = u_ac_offset_calib + u_ac_adc;
        DEL_UDC_offset_calib = DEL_UDC_offset_calib + DEL_UDC_adc;
        u_f_offset_calib = u_f_offset_calib + u_f_adc;
        u_out_offset_calib = u_out_offset_calib + u_out_adc;

        // ko potece dovolj casa, sporocim da lahko grem naprej
        // in izracunam povprecni offset
        current_offset_counter = current_offset_counter + 1;
        if (current_offset_counter == (SAMPLE_FREQ * 1L))
        {
            calibration_done = TRUE;
            start_calibration = FALSE;
            IS_offset = IS_offset_calib / (SAMPLE_FREQ*1L);
            IF_offset = IF_offset_calib / (SAMPLE_FREQ*1L);
            u_ac_offset = u_ac_offset_calib / (SAMPLE_FREQ*1L);
            DEL_UDC_offset = DEL_UDC_offset_calib / (SAMPLE_FREQ*1L);
            u_f_offset = u_f_offset_calib / (SAMPLE_FREQ*1L);
            u_out_offset = u_out_offset_calib / (SAMPLE_FREQ*1L);
        }

        IS = 0.0;
        IF = 0.0;
        u_ac = 0.0;
        DEL_UDC = 0.0;
        u_f = 0.0;
        u_out = 0.0;
    }
    else
    {
        IS = IS_gain * (IS_adc - IS_offset);
        IF = IF_gain * (IF_adc - IF_offset);
        u_ac = u_ac_gain * (u_ac_adc - u_ac_offset);
        DEL_UDC = DEL_UDC_gain * (DEL_UDC_adc - DEL_UDC_offset);
        u_f = u_f_gain * (u_f_adc - u_f_offset);
        u_out = u_out_gain * (u_out_adc - u_out_offset);
    }

    // temperatura hladilnika
    temperatura = NTC_temp();

    // porcunam DFT napetosti
    // vhodna omrežna napetost - u_ac
    u_ac_dft.In = u_ac;
    DFT_FLOAT_MACRO(u_ac_dft);

    // izhodna napetost - u_out
    u_out_dft.In = u_out;
    DFT_FLOAT_MACRO(u_out_dft);

    // naraèunam amplitudo omrežne napetosti - u_ac
    u_ac_rms = ZSQRT2 * sqrt(u_ac_dft.SumA * u_ac_dft.SumA + u_ac_dft.SumB *u_ac_dft.SumB);

    // normiram, da dobim obliko
    u_ac_form = u_ac_dft.Out / (u_ac_rms * SQRT2);

    // naraèunam amplitudo izhodne napetosti - u_out
    u_out_rms = ZSQRT2 * sqrt(u_out_dft.SumA * u_out_dft.SumA + u_out_dft.SumB *u_out_dft.SumB);

    // normiram, da dobim obliko
    u_out_form = u_out_dft.Out / (u_out_rms * SQRT2);


    // filtriram DC link napetost
    napetost_dc_f.In = DEL_UDC;
    DC_FLOAT_MACRO(napetost_dc_f);
    napetost_dc_filtered = napetost_dc_f.Mean;

    // izracunam kaksna moc je na izhodu
    //power_out = u_out * (tok_bb1 + tok_bb2);

    // ocena izhodnega toka z ABF
   // i_cap_abf.u_cap_measured = u_f;
   // ABF_float_calc(&i_cap_abf);
   // IF_abf = -i_cap_abf.i_cap_estimated + (tok_bb1 + tok_bb2);

    // zakasnim IS
    i_grid_delay.in = IS * IS_reg.Out;
    DELAY_FLOAT_CALC(i_grid_delay);

    // ocena dc toka z ABF
    //i_cap_dc.u_cap_measured = DEL_UDC;
    //ABF_float_calc(&i_cap_dc);

    // se filtriram
    //i_dc_f.In = -i_cap_dc.i_cap_estimated - i_grid_delay.out;
    //DC_FLOAT_MACRO(i_dc_f);
    //tok_dc_abf = i_dc_f.Mean;

    // filtriram tudi meritev toka
    IF_f.In = IF;
    DC_FLOAT_MACRO(IF_f);

    // statistika
    statistika.In = DEL_UDC;
    STAT_FLOAT_MACRO(statistika);
}

#pragma CODE_SECTION(NTC_temp, "ramfuncs");
float NTC_temp(void)
{
	V_NTC = M_TEMP_adc * (3.3/4096.0);
	R_NTC = 5100 * ((5/V_NTC) - 1);
	T_NTC = (T_NTC25 * beta_NTC)/(beta_NTC - T_NTC25 * log(R_NTC25/R_NTC)) - 273;

	return T_NTC;
}
