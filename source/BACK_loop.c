/****************************************************************
 * FILENAME:     BACK_loop.c
 * DESCRIPTION:  background code
 * AUTHOR:       Mitja Nemec
 *
 ****************************************************************/

#include "BACK_loop.h"

// deklaracije stati�nih lokalnih spremenljivk

bool ENABLE_tipka = FALSE;  		// pulz, ko pritisnemo na tipko ENABLE
bool RESET_tipka = FALSE;  			// pulz, ko pritisnemo na tipko RESET

bool pulse_1000ms = FALSE;
bool pulse_500ms = FALSE;
bool pulse_100ms = FALSE;
bool pulse_50ms = FALSE;
bool pulse_10ms = FALSE;

// deklaracije zunanjih spremenljivk


// pototipi funkcij

void detekcija_tipk(void);
void pulzni_gen(void);

/**************************************************************
* Funkcija, ki se izvaja v ozadju med obratovanjem
**************************************************************/
#pragma CODE_SECTION(BACK_loop, "ramfuncs");
void BACK_loop(void)
{
    // lokalne spremenljivke
    while (1)
    {
        // hendlanje komunikacije
        COMM_runtime();
		
		// test tipk in LEDic
		
		if (ENABLE_tipka == TRUE)
		{
			PCB_LED_WORKING_on();
			PCB_LED_READY_on();
		}
		else
		{
			PCB_LED_WORKING_off();
			PCB_LED_READY_off();
		}
		
		if (RESET_tipka == TRUE)
		{
			PCB_LED_FAULT_on();
		}
		else
		{
			PCB_LED_FAULT_off();
		}

        // vsake toliko �asa spremenji stanje lu�k
        if (interrupt_cnt == 0)
        {
            PCB_LEDcard_toggle();
            // in pocakam najmanj 50 us - da ne bi slucajno izvedel tega dvakrat
            DELAY_US(50L);
			
        }

        asm(" NOP");
    }   // end of while(1)
}       // end of BACK_loop

void pulzni_gen(void)
{
    static long interrupt_cnt_old_500ms = 0;
    static long interrupt_cnt_old_100ms = 0;
    static long interrupt_cnt_old_50ms = 0;
    static long interrupt_cnt_old_10ms = 0;
    static int  pulse_1000ms_cnt = 0;

    long delta_cnt_500ms;
    long delta_cnt_100ms;
    long delta_cnt_50ms;
    long delta_cnt_10ms;

    if ( (interrupt_cnt - interrupt_cnt_old_500ms) < 0)
    {
        interrupt_cnt_old_500ms = interrupt_cnt_old_500ms - (SAMPLE_FREQ);
    }

    if ( (interrupt_cnt - interrupt_cnt_old_100ms) < 0)
    {
        interrupt_cnt_old_100ms = interrupt_cnt_old_100ms - (SAMPLE_FREQ);
    }

    if ( (interrupt_cnt - interrupt_cnt_old_50ms) < 0)
    {
        interrupt_cnt_old_50ms = interrupt_cnt_old_50ms - (SAMPLE_FREQ);
    }

    if ( (interrupt_cnt - interrupt_cnt_old_10ms) < 0)
    {
        interrupt_cnt_old_10ms = interrupt_cnt_old_10ms - (SAMPLE_FREQ);
    }

    delta_cnt_500ms = interrupt_cnt - interrupt_cnt_old_500ms;
    delta_cnt_100ms = interrupt_cnt - interrupt_cnt_old_100ms;
    delta_cnt_50ms = interrupt_cnt - interrupt_cnt_old_50ms;
    delta_cnt_10ms = interrupt_cnt - interrupt_cnt_old_10ms;

    // generiranje pulza vsakih 10ms
    if (delta_cnt_10ms > ((SAMPLE_FREQ)/100))
    {
        pulse_10ms = TRUE;
        interrupt_cnt_old_10ms = interrupt_cnt;
    }
    else
    {
        pulse_10ms = FALSE;
    }

    // generiranje pulza vsakih 50ms
    if (delta_cnt_50ms > ((SAMPLE_FREQ)/50))
    {
        pulse_50ms = TRUE;
        interrupt_cnt_old_50ms = interrupt_cnt;
    }
    else
    {
        pulse_50ms = FALSE;
    }


    // generiranje pulza vsakih 100ms
    if (delta_cnt_100ms > ((SAMPLE_FREQ)/10))
    {
        pulse_100ms = TRUE;
        interrupt_cnt_old_100ms = interrupt_cnt;
    }
    else
    {
        pulse_100ms = FALSE;
    }

    // generiranje pulza vsakih 500ms
    if (delta_cnt_500ms > ((SAMPLE_FREQ)/2))
    {
        pulse_500ms = TRUE;
        interrupt_cnt_old_500ms = interrupt_cnt;
    }
    else
    {
        pulse_500ms = FALSE;
    }

    // stejem pulze po 100ms, da dobimo pulz 1s
    if (pulse_100ms == TRUE)
    {
        pulse_1000ms_cnt = pulse_1000ms_cnt + 1;
        if (pulse_1000ms_cnt == 10)
        {
            pulse_1000ms = TRUE;
            pulse_1000ms_cnt = FALSE;
        }
        else
        {
            pulse_1000ms = FALSE;
        }
    }
    // preverjanje da pulz traja samo eno iteracijo
    if ((pulse_100ms != TRUE) && (pulse_1000ms == TRUE))
    {
        pulse_1000ms = FALSE;
    }
}

void detekcija_tipk(void)
{
    // lokalne spremenljivke
    const int sw_on_cnt_limit = 10;

    static int ENABLE_tipka_cnt = 0;
    static int RESET_tipka_cnt = 0;

    bool ENABLE_tipka_new;
    bool RESET_tipka_new;

    // scan every cca 0.05s
    if (pulse_10ms == 1)
    {
        // preberem stanja tipk
        ENABLE_tipka_new = PCB_SW_ENABLE();
        RESET_tipka_new = PCB_SW_RESET();

        // ali smo pritisnili na tipko ENABLE
        // ce je tipka pritisnjena, stopaj koliko casa je prisitsnjena
        if (ENABLE_tipka_new == TRUE)
        {
            ENABLE_tipka_cnt = ENABLE_tipka_cnt + 1;
        }
        // ce ni pritisnjena resetiraj stevec
        else
        {
            ENABLE_tipka_cnt = 0;
        }

        // ce je tipka pritisnjena dovolj casa, javi programu - samo enkrat
        if (ENABLE_tipka_cnt == sw_on_cnt_limit)
        {
            ENABLE_tipka = TRUE;
        }
        // sicer pa ne javi
        else
        {
            ENABLE_tipka = FALSE;
        }

        // ali smo pritisnili na tipko 2
        // ce je tipka pritisnjena, stopaj koliko casa je prisitsnjena
        if (reset_tipka_new == TRUE)
        {
            reset_tipka_cnt = reset_tipka_cnt + 1;
        }
        // ce ni pritisnjena resetiraj stevec
        else
        {
            reset_tipka_cnt = 0;
        }

        // ce je tipka pritisnjena dovolj casa, javi programu - samo enkrat
        if (reset_tipka_cnt == sw_on_cnt_limit)
        {
            reset_tipka = TRUE;
        }
        // sicer pa ne javi
        else
        {
            reset_tipka = FALSE;
        }
    }
    // da je pulz dolg res samo in samo eno iteracijo
    else
    {
        en_tipka = FALSE;
        reset_tipka = FALSE;
    }
}