/****************************************************************
* FILENAME:     define.h           
* DESCRIPTION:  file with global define macros
* AUTHOR:       Mitja Nemec
*
****************************************************************/
#ifndef     __DEFINE_H__
#define     __DEFINE_H__

#include	"stddef.h"
#include	"stdbool.h"
#include	"stdint.h"

// frekvenca PWM-ja (20kHz)
#define     SWITCH_FREQ     20000L

// ve�kratnik preklopne frekvence
#define     SAMPLE_PRESCALE   1

// razmerje med preklopno in vzor�no frekvenco
#define     SAMPLING_RATIO  2

// Vzor�na frekvenca (20kHz)
#define     SAMPLE_FREQ     (SWITCH_FREQ/SAMPLE_PRESCALE)

// vzor�na perioda
#define     SAMPLE_TIME     (1.0/SAMPLE_FREQ)

// frekvenca omre�ja
#define		GRID_FREQ		50

// amplituda omrezja
#define		GRID_AMPLITUDE	325.2691193

// �tevilo vzorcev v eni periodi
#define		SAMPLE_POINTS	(SAMPLE_FREQ/GRID_FREQ)

// frekvenca procesorja v Hz (200 MHz)
#define     CPU_FREQ        200000000L

// definicije matemati�nih konstant
#define     SQRT3           1.7320508075688772935274463415059
#define     SQRT2           1.4142135623730950488016887242097
#define     ZSQRT2          0.70710678118654752440084436210485
#define     PI              3.1415926535897932384626433832795

// nastavljena napetost enosmernega tokokroga
#define     DEL_UDC_REF        	40.0

// limitne vrednosti napetosti, tokov
#define     IS_LIM				6.0
#define     IF_LIM				25.0
#define     u_ac_RMS_MIN		196.0
#define     u_ac_RMS_MAX		253.0
#define     DEL_UDC_MAX			47.0
#define     DEL_UDC_MIN			-0.1
#define		u_f_LIM				40.0

// deklaracije za logicne operacije
#define     TRUE            true
#define     FALSE           false

// kako naj se obna�ajo preriferne naprave, ko ustavimo izvajanje programa
// 0 stop immediately, 1 stop when finished, 2 run free
#define     DEBUG_STOP      0

#endif // end of __DEFINE_H__ definition
