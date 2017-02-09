/****************************************************************
 * FILENAME:     BACK_loop.c
 * DESCRIPTION:  background code
 * AUTHOR:       Mitja Nemec
 *
 ****************************************************************/

#include "BACK_loop.h"

// deklaracije statiènih lokalnih spremenljivk


// deklaracije zunanjih spremenljivk


// pototipi funkcij

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

        // vsake toliko èasa spremenji stanje luèk
        if (interrupt_cnt == 0)
        {
            PCB_LEDcard_toggle();
            // in pocakam najmanj 50 us - da ne bi slucajno izvedel tega dvakrat
            DELAY_US(50L);
        }

        asm(" NOP");
    }   // end of while(1)
}       // end of BACK_loop

