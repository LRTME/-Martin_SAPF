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

// veèkratnik preklopne frekvence
#define     SAMP_PRESCALE   1

// razmerje med preklopno in vzorèno frekvenco
#define     SAMPLING_RATIO  2

// Vzorèna frekvenca (20kHz)
#define     SAMPLE_FREQ     (SWITCH_FREQ/SAMP_PRESCALE)

// vzorèna perioda
#define     SAMPLE_TIME     (1.0/SAMPLE_FREQ)

// frekvenca omrežja
#define		GRID_FREQ		50

// število vzorcev v eni periodi
#define		SAMPLE_POINTS	(SAMPLE_FREQ/GRID_FREQ)

// frekvenca procesorja v Hz (200 MHz)
#define     CPU_FREQ        200000000L

// definicije matematiènih konstant
#define     SQRT3           1.7320508075688772935274463415059
#define     SQRT2           1.4142135623730950488016887242097
#define     ZSQRT2          0.70710678118654752440084436210485
#define     PI              3.1415926535897932384626433832795

// deklaracije za logicne operacije
#define     TRUE            true
#define     FALSE           false

// kako naj se obnašajo preriferne naprave, ko ustavimo izvajanje programa
// 0 stop immediately, 1 stop when finished, 2 run free
#define     DEBUG_STOP      0

#endif // end of __DEFINE_H__ definition
