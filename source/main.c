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

labela:
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

    // inicializiram ADC in moènostni modul
    ADC_init();

    FB1_init();
    FB2_init();

    // inicializiram peridoièno prekinitev za regulacijo
    PER_int_setup();

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
    fault_flags.undervoltage_DEL_UDC = FALSE;
    fault_flags.overvoltage_DEL_UDC = FALSE;
    fault_flags.undervoltage_u_ac = FALSE;
    fault_flags.overvoltage_u_ac = FALSE;
    fault_flags.cpu_overrun = FALSE;
    fault_flags.fault_registered = FALSE;
    fault_flags.HW_trip = FALSE;


    // pocakam, da se izvede kalibracija tokovnih sond
        DELAY_US(10000);
        start_calibration = TRUE;
        while(calibration_done == FALSE)
        {
            /* DO NOTHING */
        }


    /* vklopna procedura */
        state = Startup;
        // najprej se priklopin na omrežje in poèasi napolnim DC-link
        PCB_relay1_on();
        DELAY_US(1000000);

        // èe se je vklopila strojna zašèita se je zelo verjetno vklopila zaradi prevelikega toka
        if (PCB_CPLD_trip() == FALSE)
        {
            // zato kar resetiram MCU, da se zaženemo še enkrat
            asm(" ESTOP0");
            DINT;
            goto labela;

            /*
            asm(" ESTOP0");
            EALLOW;
            SysCtrlRegs.WDCR = 0x0040;
            EDIS;
			*/
        }


        // pocakam, da napetost na enosmernem tokokrogu naraste
/*        while (DEL_UDC < u_ac_rms * SQRT2 * (24 / 230))
        {
              DO NOTHING
        }
*/
        // kratkostièim zagonski upor R1 (100R)
        PCB_relay2_on();
        DELAY_US(1000000);

        // in pocakam, da napetost na enosmernem tokokrogu naraste do konca
/*        while (DEL_UDC < u_ac_rms * SQRT2 * (24 / 230))
        {
             DO NOTHING
        }
*/
        //vklopim moènostno stopnjo in povem regulaciji da zaène delati

        DINT;

        FB1_enable();
        state = Standby;
        // zeljeno vrednost enaccim z trenutno, da se lepo zapeljem po rampi
        DEL_UDC_slew.Out = DEL_UDC;
        EINT;
        // pocakam da se napetost enosmernega kroga zapelje na nastavljeno vrednost
        while(fabs(DEL_UDC_reg.Fdb - DEL_UDC_reg.Ref) > 0.1)
        {
           /* DO NOTHING */
        }

        // grem v neskoncno zanko, ki se izvaja v ozadju
        BACK_loop();

}   // end of main

