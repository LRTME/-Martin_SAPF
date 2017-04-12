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

    // inicializiram ADC in moènostni modul
    	ADC_init();
    	FB1_init();
    	FB2_init();

    // inicializiram peridoièno prekinitev za regulacijo
    	PER_int_setup();

    // inicializacija komunikacijoe
    	COMM_initialization();

    // zagon PWM enot
    	FB1_start();
    	FB2_start();

    // omogocim prekinitve
    	EINT;
    	ERTM;

    // pocakam, da se izvede par prekinitev, da zacnem brcati psa cuvaja
    	DELAY_US(1000);

    // resetiram zapahe
    	PCB_CPLD_LATCH_RESET();

    // resetiram vse morebitne napake
    	fault_flags.overcurrent_IF = FALSE;
    	fault_flags.overcurrent_IS = FALSE;
    	fault_flags.undervoltage_u_dc = FALSE;
    	fault_flags.overvoltage_u_dc = FALSE;
    	fault_flags.undervoltage_u_ac = FALSE;
    	fault_flags.overvoltage_u_ac = FALSE;
    	fault_flags.cpu_overrun = FALSE;
    	fault_flags.fault_registered = FALSE;
    	fault_flags.HW_trip = FALSE;

    // inicializiram zašèitno prekinitev,
    // in sicer po tem ko resetiram latch
    // saj bi se mi v nasprotnem primeru izvedla zašèitna prekinitev
    	FLT_int_setup();

    // pocakam, da se izvede kalibracija tokovnih sond
        DELAY_US(10000);
        start_calibration = TRUE;
        while(calibration_done == FALSE)
        {
           	/* DO NOTHING */
       	}


    // vklopna procedura
        state = Startup;
        // najprej se priklopim na omrezje preko zagonskega upora R1 in pocasi napolnim DC-link
        PCB_relay1_on();
        DELAY_US(1000000);

        // èe se je vklopila strojna zašèita se je zelo verjetno vklopila zaradi prevelikega toka
        if (PCB_CPLD_trip() == FALSE)
        {
            // zato kar resetiram MCU, da se zaženemo še enkrat
            asm(" ESTOP0");

            // èe pa teèem bez debuggerja, pa se tukaj zaustavim
            while(1)
            {
            	// DO NOTHING
            }
        }

        // pocakam, da napetost na enosmernem tokokrogu naraste
        while (u_dc < u_ac_rms * SQRT2 * (24 / 230))
        {
             // DO NOTHING
        }

        // kratkostièim zagonski upor R1 (470R)
        PCB_relay2_on();
        DELAY_US(1000000);

        // in pocakam, da napetost na enosmernem tokokrogu naraste do konca
        while (u_dc < u_ac_rms * SQRT2 * (24 / 230))
        {
           // DO NOTHING
        }

        //vklopim moènostno stopnjo in povem regulaciji da zaène delati

        DINT;
        FB1_enable();
        //
        state = Standby;
        // zeljeno vrednost enacim s trenutno u_dc, da se lepo zapeljem po rampi
        u_dc_slew.Out = u_dc;
        EINT;


        // pocakam da se napetost enosmernega kroga zapelje na nastavljeno vrednost (DEL_UDC_REF)
        while(fabs(u_dc_reg.Fdb - u_dc_reg.Ref) > 0.1)
        {
            // DO NOTHING
        }

        // grem v neskoncno zanko, ki se izvaja v ozadju
        BACK_loop();

}   // end of main

