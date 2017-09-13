/****************************************************************
 * FILENAME:     BACK_loop.c
 * DESCRIPTION:  background code
 * AUTHOR:       Mitja Nemec
 *
 ****************************************************************/

#include "BACK_loop.h"

// deklaracije statiènih lokalnih spremenljivk
// deklaracije lokalnih spremenljivk
bool enable_tipka = FALSE;
bool reset_tipka = FALSE;

int sw_on_cnt_limit = 10;

bool pulse_1000ms = FALSE;
bool pulse_500ms = FALSE;
bool pulse_100ms = FALSE;
bool pulse_50ms = FALSE;
bool pulse_10ms = FALSE;

// deklaracije zunanjih spremenljivk

int pulse_10ms_cnt = 0;
int pulse_10ms_cnt_previous = 0;
int delay_MOSFET_relay_10ms = 5;		// 50 ms

// pototipi funkcij

void scan_keys(void);
void pulse_gen(void);

void startup_fcn(void);
void standby_fcn(void);
void enable_fcn(void);
void working_fcn(void);
void disable_fcn(void);
void fault_sensed_fcn(void);
void fault_fcn(void);


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

        pulse_gen();
        scan_keys();

		/*	state machine	*/

		switch(state)
		{
        case Startup:
            startup_fcn();
            break;
		case Standby:
			standby_fcn();
			break;
		case Enable:
			enable_fcn();
			break;
		case Working:
			working_fcn();
			break;
		case Disable:
			disable_fcn();
			break;
		case Fault_sensed:
		    fault_sensed_fcn();
		    break;
		case Fault:
			fault_fcn();
			break;
		}

        asm(" NOP");

        // end of while(1)
	}
}// end of BACK_loop



#pragma CODE_SECTION(startup_fcn, "ramfuncs");
void startup_fcn(void)
{
    static unsigned int substate = 0;
    static unsigned int charge_cnt = 0;

    // najprej se priklopim na omrezje preko zagonskega upora R1 in pocasi napolnim DC-link
    if (substate == 0)
    {
        PCB_relay1_on();
        substate = substate + 1;
    }

    // nato pocakam, da se napolni
    if (substate == 1)
    {
        if (pulse_10ms == TRUE)
        {
            charge_cnt = charge_cnt + 1;
        }
        if (charge_cnt >= 100)
        {
            charge_cnt = 0;
            substate = 2;
        }
    }

    // nato preverim ali sem se dovolj napolnil
    if (substate == 2)
    {
        if (u_dc > u_ac_rms * SQRT2 * (24 / 230))
        {
            substate = 3;
        }
    }

    // nato brikam zagonske upore
    if (substate == 3)
    {
        PCB_relay2_on();
        substate = 4;
    }

    // nato pocakam, da se napolni
    if (substate == 4)
    {
        if (pulse_10ms == TRUE)
        {
            charge_cnt = charge_cnt + 1;
        }
        if (charge_cnt >= 100)
        {
            charge_cnt = 0;
            substate = 5;
        }
    }

    // nato preverim ali sem se dovolj napolnil
    if (substate == 5)
    {
        if (u_dc > u_ac_rms * SQRT2 * (24 / 230))
        {
            substate = 6;
        }
    }

    //vklopim moènostno stopnjo in povem regulaciji da zaène delati
    if (substate == 6)
    {
        DINT;
        // zeljeno vrednost enacim s trenutno u_dc, da se lepo zapeljem po rampi
        u_dc_slew.Out = u_dc;
        // omogocim mocnostno stopnjo
        FB1_enable();
        state == Charging;
        EINT;
        substate = 7;
    }

    // pocakam, da DC napetost dovolj naraste
    if (substate == 7)
    {
        if (fabs(u_dc_reg.Fdb - u_dc_reg.Ref) < 0.1)
        {
            state = Standby;
            // pripravljen za ponoven zagon
            substate = 0;
        }
    }

    // signalizacija
    PCB_LED_FAULT_off();
    PCB_LED_WORKING_off();

    // signalizacija
    if (pulse_100ms == TRUE)
    {
        PCB_LED_READY_toggle();
    }

}

#pragma CODE_SECTION(standby_fcn, "ramfuncs");
void standby_fcn(void)
{
	if(enable_tipka == TRUE)
	{
		state = Enable;
		enable_tipka = FALSE;
	}

	//signalizacija
    PCB_LED_READY_on();
    PCB_LED_WORKING_off();

}

#pragma CODE_SECTION(enable_fcn, "ramfuncs");
void enable_fcn(void)
{

	// vkljucim MOSFET in izkljucim rele 3 ter nastavim stevec
	if (	(PCB_CPLD_MOSFET_MCU_status() == FALSE)
		&&	(PCB_relay3_status() == FALSE)
		&&	(FB2_status() == FB_DIS)				)
	{
		PCB_CPLD_MOSFET_MCU_on();
		PCB_relay3_on();
		FB2_enable();


		// nastavim stevec
		if(pulse_10ms_cnt <= (99 - delay_MOSFET_relay_10ms))
		{
			pulse_10ms_cnt_previous = pulse_10ms_cnt;
		}
		else
		{
			pulse_10ms_cnt_previous = pulse_10ms_cnt - 99;
		}
	}

	// po 50 ms dam znak, da se lahko v PER_int vkljuci FB2 (ob kotu 0°) in izklopi MOSFET
	if (	((pulse_10ms_cnt - pulse_10ms_cnt_previous) >= delay_MOSFET_relay_10ms)
		&&	(PCB_CPLD_MOSFET_MCU_status() == TRUE)
		&&	(PCB_relay3_status() == TRUE)
		&&	(FB2_status() == FB_EN)						)
	{
		enable = TRUE;
	}

	// signalizacija
	if (pulse_100ms == TRUE)
	{
	    PCB_LED_WORKING_toggle();
	}
}

#pragma CODE_SECTION(working_fcn, "ramfuncs");
void working_fcn(void)
{
	if(enable_tipka == TRUE)
	{
		state = Disable;
	}

	// signalizacija
	PCB_LED_WORKING_on();

}

#pragma CODE_SECTION(disable_fcn, "ramfuncs");
void disable_fcn(void)
{
	// postane aktivno sele po izklopnem delu v PER_int (izklop pri kotu 0°), disable == TRUE
	if (		(PCB_CPLD_MOSFET_MCU_status() == TRUE)
			&&	(PCB_relay3_status() == FALSE)
			&&	(FB2_status() == FB_DIS)
			&&	(disable == TRUE)					)

	{
		// ponastavim stevec
		if(pulse_10ms_cnt <= (99 - delay_MOSFET_relay_10ms))
		{
			pulse_10ms_cnt_previous = pulse_10ms_cnt;
		}
		else
		{
			pulse_10ms_cnt_previous = pulse_10ms_cnt - 99;
		}

		// pobrisem zastavico
		disable = FALSE;

	}

	// po 50ms, ko rele zagotovo preklopi, ugasnemo kratkosticna MOSFET-a
	if (		((pulse_10ms_cnt - pulse_10ms_cnt_previous) >= delay_MOSFET_relay_10ms)
			&&	(PCB_CPLD_MOSFET_MCU_status() == TRUE)
			&&	(PCB_relay3_status() == FALSE)
			&&	(FB2_status() == FB_DIS)
			&&	(disable == FALSE)						)
	{
		PCB_CPLD_MOSFET_MCU_off();
		state = Standby;
	}

    // signalizacija
    if (pulse_100ms == TRUE)
    {
        PCB_LED_WORKING_toggle();
    }
}

#pragma CODE_SECTION(fault_fcn, "ramfuncs");
void fault_fcn(void)
{
    // pobrišem napako, in grem v standby
    if (reset_tipka == TRUE)
    {
    	PCB_LED_FAULT_off();

    	// resetiram zapahe
    	PCB_CPLD_LATCH_RESET();

    	// resetiram vse morebitne napake
    	fault_flags.overcurrent_IF = FALSE;
    	fault_flags.overcurrent_IS = FALSE;
    	fault_flags.undervoltage_u_dc = FALSE;
    	fault_flags.overvoltage_u_dc = FALSE;
    	fault_flags.undervoltage_u_ac = FALSE;
    	fault_flags.overvoltage_u_ac = FALSE;
    	fault_flags.overvoltage_u_f = FALSE;
    	fault_flags.cpu_overrun = FALSE;
    	fault_flags.fault_registered = FALSE;
    	fault_flags.HW_trip = FALSE;

    	// inicializiram zašèitno prekinitev,
    	// in sicer po tem ko resetiram latch
    	// saj bi se mi v nasprotnem primeru izvedla zašèitna prekinitev
    	FLT_int_setup();

    	// vklopim napajanje za ISO-te
    	PCB_5V_ISO_on();

    	// omogocim FLT prekinitev
    	FLT_int_enable();

        // vklopna procedura
        state = Startup;
    }
    // signalizacija
    PCB_LED_FAULT_on();
}

#pragma CODE_SECTION(fault_sensed_fcn, "ramfuncs");
void fault_sensed_fcn(void)
{
    // izklopim mostic
    FB1_disable();
    FB2_disable();

    // izklopim 5V_ISO linijo
    PCB_5V_ISO_off();

    // izklopim vse kontaktorjev
    PCB_relay1_off();
    PCB_relay2_off();
    PCB_relay3_off();

    FLT_int_disable();

    if (fault_flags.fault_registered == FALSE)
    {
        fault_flags.fault_registered = TRUE;
    }
    // signalizacija
    PCB_LED_WORKING_off();
    PCB_LED_READY_off();

    state = Fault;
}

void scan_keys(void)
{
    // lokalne spremenljivke
    static int enable_tipka_cnt = 0;
    static int reset_tipka_cnt = 0;

    bool enable_tipka_new;
    bool reset_tipka_new;

    // scan every cca 0.05s
    if (pulse_10ms == 1)
    {
        // preberem stanja tipk
        enable_tipka_new = PCB_SW_ENABLE();
        reset_tipka_new = PCB_SW_RESET();

        // ali smo pritisnili na tipko 1
        // ce je tipka pritisnjena, stopaj koliko casa je prisitsnjena
        if (enable_tipka_new == TRUE)
        {
            enable_tipka_cnt = enable_tipka_cnt + 1;
        }
        // ce ni pritisnjena resetiraj stevec
        else
        {
            enable_tipka_cnt = 0;
        }

        // ce je tipka pritisnjena dovolj casa, javi programu - samo enkrat
        if (enable_tipka_cnt == sw_on_cnt_limit)
        {
            enable_tipka = TRUE;
        }
        // sicer pa ne javi
        else
        {
            enable_tipka = FALSE;
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
        enable_tipka = FALSE;
        reset_tipka = FALSE;
    }
}

void pulse_gen(void)
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

    // stejem pulze po 10ms
        if (pulse_10ms == TRUE)
        {
            pulse_10ms_cnt = pulse_10ms_cnt + 1;

            // resetiram stevec po 1000ms
            if (pulse_10ms_cnt == 100)
            {
                pulse_10ms_cnt = 0;
            }
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
