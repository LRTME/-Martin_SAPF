/************************************************************** 
* FILE:         PWM_drv.h
* DESCRIPTION:  PWM driver
* AUTHOR:       Mitja Nemec
*
****************************************************************/
#ifndef     __PWM_DRV_H__
#define     __PWM_DRV_H__

#include    "F28x_Project.h"

#include    "define.h"
#include    "globals.h"

/* Definicije */
// delovanje modula ob debug-dogodkih
// 0 stop immediately, 1 stop when finished, 2 run free
#define     PWM_DEBUG       0

// frekvenca PWM enote
#define		PWM_TMR_FREQ	(CPU_FREQ/2)

// perioda casovnika (v procesorskih cikilh) 
#define     PWM_PERIOD      (PWM_TMR_FREQ/(SAMPLE_FREQ * SAMPLE_PRESCALE))

// prescaler za prekinitev
#define     PWM_INT_PSCL    SAMPLE_PRESCALE

/**************************************************************
* Funkcija, poklièe funkciji PWM_PWM_init in PWM_ADC_init; klièemo
* jo iz main-a
* return: void
**************************************************************/
extern void PWM_init(void);

/**************************************************************
* Funkcija, ki popiše registre za PWM1,. Znotraj funkcije
* se omogoèi interrupt za proženje ADC, popiše se perioda, compare
* register, omogoèi se izhode za PWM...
* return:void
**************************************************************/
extern void PWM_update(float duty);

/**************************************************************
* Funkcija, ki starta PWM1. Znotraj funkcije nastavimo
* naèin štetja èasovnikov (up-down-count mode)
* return: void
**************************************************************/
extern void PWM_start(void);

/**************************************************************
* Funkcija, ki nastavi periodo, za doseganje zeljene periode
* in je natancna na cikel natancno
* return: void
* arg1: zelena perioda
**************************************************************/
extern void PWM_period(float perioda);

/**************************************************************
* Funkcija, ki nastavi periodo, za doseganje zeljene frekvence
* in je natancna na cikel natancno
* return: void
* arg1: zelena frekvenca
**************************************************************/
extern void PWM_frequency(float frekvenca);

#endif  // end of __PWM_DRV_H__ definition

