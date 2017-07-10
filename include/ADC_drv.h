/**************************************************************
* FILE:         ADC_drv.h
* DESCRIPTION:  A/D driver
* AUTHOR:       Mitja Nemec
*
****************************************************************/
#ifndef     __ADC_DRV_H__
#define     __ADC_DRV_H__

#include    "F28x_Project.h"
#include    "define.h"

// s kakšnim taktom PWM-ja naj prožim ADC
#define     ADC_SAMP_RATIO  SAMPLING_RATIO

// doloci kateri PWM modul prozi ADC
#define     ADC_MODUL1      EPwm1Regs

// kje se nahajajo rezultati

#define     u_f_adc	  		(AdcaResultRegs.ADCRESULT0)		//A0
#define     u_out_adc  		(AdcaResultRegs.ADCRESULT1)		//A1
#define     u_ac_adc       	(AdcaResultRegs.ADCRESULT3)		//A3

#define     IF_adc 			(AdcbResultRegs.ADCRESULT0)		//B0
#define     DEL_UDC_adc		(AdcbResultRegs.ADCRESULT1)		//B1
#define     IS_adc 			(AdcbResultRegs.ADCRESULT3)		//B3

#define     I_out_adc     	(AdcdResultRegs.ADCRESULT0)		//D0

/**************************************************************
* inicializiramo ADC
**************************************************************/
extern void ADC_init(void);

/**************************************************************
* Funkcija, ki pocaka da ADC konca s pretvorbo
* vzorcimo...
* return: void
**************************************************************/
extern void ADC_wait(void);

#endif /* __ADC_DRV_H__ */
