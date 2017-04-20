/************************************************************** 
* FILE:         FB_bipolar.h
* DESCRIPTION:  header file for full bridg driver
* AUTHOR:       Mitja Nemec
* START DATE:   18.8.2010
* VERSION:      1.0
*
* CHANGES : 
* VERSION   DATE        WHO                 DETAIL 
* 1.0       21.12.2009  Mitja Nemec         Initial version
*
****************************************************************/
#ifndef     __FB_BIPOLAR_H__
#define     __FB_BIPOLAR_H__

#include    "F28x_Project.h"

#include    "define.h"
#include    "globals.h"
#include	"main.h"


/* Definicije */
// stevilke PWM modulov, ki krmilijo mostic
// ne pozabi spremeniti GPIO mux registrov
#define     FB1_MODUL1        EPwm1Regs
#define     FB1_MODUL2        EPwm2Regs
#define     FB2_MODUL1        EPwm3Regs
#define     FB2_MODUL2        EPwm4Regs

// delovanje modula ob debug-dogodkih
// (0-stop immediately, 1-stop at zero, 2-run free)
#define     FB1_DEBUG        2
#define     FB2_DEBUG        2

// perioda casovnika (v procesorskih cikilh) 
#define     FB_PERIOD       (((CPU_FREQ/2)/SWITCH_FREQ)/2 - 1)   //50us

// dead time (v procesorskih cikilh)
#define     FB_DEAD_TIME    (40)

// definicije za status mosticev
enum FB_STATE { FB_DIS=0, FB_EN, FB_BOOTSTRAP, FB_TRIP};

/**************************************************************
* nastavi mrtvi cas
**************************************************************/
extern void FB1_dead_time(float dead_time);
extern void FB2_dead_time(float dead_time);

/**************************************************************
* Izklopi zgornja tranzistorja in vklopi spodnja, tako da je mostic kratkosticen
* in lahko napolneta bootstrap kondenzatorja
* returns:  
**************************************************************/
extern void FB1_bootstrap(void);
extern void FB2_bootstrap(void);

/**************************************************************
* Izklopi vse tranzistorje - urgentno
* returns:
**************************************************************/
extern void FB1_trip(void);
extern void FB2_trip(void);

/**************************************************************
* Izklopi vse tranzistorje
* returns:  
**************************************************************/
extern void FB1_disable(void);
extern void FB2_disable(void);

/**************************************************************
* vklopi vse izhode
* returns:  
**************************************************************/
extern void FB1_enable(void);
extern void FB2_enable(void);

/**************************************************************
* Funkcija, ki popiše registre za PWM1,. Znotraj funkcije
* se omogoèi interrupt za proženje ADC, popiše se perioda, compare
* register, omogoèi se izhode za PWM...
* return:void
**************************************************************/
extern void FB1_init(void);
extern void FB2_init(void);

/**************************************************************
* Funkcija, ki osveži registre za PWM1
* return: void
**************************************************************/
extern void FB1_update(float duty);
extern void FB2_update(float duty);

/**************************************************************
* Funkcija, ki starta PWM1. Znotraj funkcije nastavimo
* naèin štetja èasovnikov (up-down-count mode)
* return: void
**************************************************************/
extern void FB1_start(void);
extern void FB2_start(void);

/**************************************************************
* Funkcija, ki nastavi periodo, za doseganje zeljene periode
* in je natancna na cikel natanèno
* return: void
* arg1: zelena perioda
**************************************************************/
extern void FB1_period(float perioda);
extern void FB2_period(float perioda);

/**************************************************************
* Funkcija, ki nastavi periodo, za doseganje zeljene frekvence
* in je natancna na cikel natanèno
* return: void
* arg1: zelena frekvenca
**************************************************************/
extern void FB1_frequency(float frekvenca);
extern void FB2_frequency(float frekvenca);

/**************************************************************
* return:status
**************************************************************/
extern int FB1_status(void);
extern int FB2_status(void);


#endif  // end of __FB_BIPOLAR_H__ definition

