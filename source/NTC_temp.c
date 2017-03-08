/*
 * NTC_temp.c
 *
 *  Created on: 8. mar. 2017
 *      Author: Martin Rozej
 */
#include "NTC_temp.h"
// NTC
float beta_NTC = 3988;
float R_NTC25 = 5000;
float T_NTC25 = 298;
float V_NTC = 0.0;
float R_NTC = 0.0;
float T_NTC = 0.0;

#pragma CODE_SECTION(NTC_temp, "ramfuncs");
void NTC_temp(void)
{
	V_NTC = M_TEMP_adc * (3.3/4096.0);
	R_NTC = 5100 * ((5/V_NTC) - 1);
	T_NTC = (T_NTC25 * beta_NTC)/(beta_NTC - T_NTC25 * log(R_NTC25/R_NTC)) - 273;
}

