/****************************************************************
* FILENAME:     main.c
* DESCRIPTION:  initialization code
* AUTHOR:       Mitja Nemec
*
****************************************************************/
#include    "main.h"

/**************************************************************
* Funkcija, ki se izvede inicializacijo
**************************************************************/
void main(void)
{
    // PLL, in ura
    InitSysCtrl();
    EALLOW;
    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 0;
    EDIS;
    // GPIO - najprej
    InitGpio();

    // inicializiram vhodno izhodne pine
    PCB_init();

    // generic init of PIE
    InitPieCtrl();

    // basic vector table
    InitPieVectTable();

    // inicializacija komunikacijoe
    COMM_initialization();

    // inicializiram ADC in PWM modul
    ADC_init();
    PWM_init();
    FB_init();

    // inicializiram peridoièno prekinitev za regulacijo motorja
    PER_int_setup();

    // zagon PWM enot
    FB_start();

    // omogocim prekinitve
    EINT;
    ERTM;

    // pozenem casovnik, ki bo prozil ADC in prekinitev
    PWM_start();

    // grem v neskoncno zanko, ki se izvaja v ozadju
    BACK_loop();
}   // end of main

