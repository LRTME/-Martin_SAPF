/**************************************************************
* FILE:         PCB_util.c 
* DESCRIPTION:  PCB initialization & Support Functions
* AUTHOR:       Mitja Nemec
*
**************************************************************/
#include "PCB_util.h"

/**************************************************************
* WatchDog
**************************************************************/

// frekvenca s katero brcnemo Watch dog (Hz)
#define PCB_WD_FREQ    1000
// trajanje brce (us)
#define PCB_WD_WIDTH   100L

#define PCB_WD_KICK_ON ((SAMPLE_FREQ * PCB_WD_WIDTH) / 1000000L)
// za merjenje casa za brcanje Watch dog
static unsigned int  counter = 0;

/**************************************************************
* Funckija ki brcne zunanji WatchDog se lahko
* kliče samo iz periodicne prekinitve
**************************************************************/
#pragma CODE_SECTION(PCB_WD_KICK_int, "ramfuncs");
void PCB_WD_KICK_int(void)
{
    // brcnemo Watch dog
    if (counter <= PCB_WD_KICK_ON)
    {
        GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;
    }
    else
    {
        GpioDataRegs.GPBSET.bit.GPIO33 = 1;
    }

    // povecamo stevec prekinitev
    counter = counter + 1;
    if (counter > (SAMPLE_FREQ / PCB_WD_FREQ) )
    {
        counter = 0;
    }
}
/**************************************************************
* Funkcije za branje signalov s CPLD
***************************************************************
* Funckija, ki vrne stanje "trip" s CPLD (trip)
**************************************************************/
bool PCB_CPLD_trip(void)
{
	if (GpioDataRegs.GPADAT.bit.GPIO14 == 1)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

/**************************************************************
* Funckija, ki vrne stanje "over_voltage" z CPLD (over_voltage)
**************************************************************/
//bool PCB_CPLD_over_voltage(void)

/**************************************************************
* Funckija, ki vrne stanje "over_current_supply" s CPLD
* (over_current_supply)
**************************************************************/
//bool PCB_CPLD_over_current_supply(void)

/**************************************************************
* Funckija, ki vrne stanje "over_current_filter" s CPLD
* (over_current_filter)
**************************************************************/
//bool PCB_CPLD_over_current_filter(void)


/**************************************************************
* Funkcije izhodov na CPLD
***************************************************************
* Funckija izhoda MOSFET_MCU
**************************************************************/
void PCB_CPLD_MOSFET_MCU(void)
{
	GpioDataRegs.GPASET.bit.GPIO25 = 1;
}

/**************************************************************
* Funckija izhoda LATCH_RESET
**************************************************************/
void PCB_CPLD_LATCH_RESET(void)
{
	GpioDataRegs.GPASET.bit.GPIO27 = 1;
}
/*************************************************************/


/**************************************************************
* Funkcije za vklop/izklop relejev (preko CPLD)
***************************************************************
* Funckija, ki vklopi Rele1 (Supply_main_relay)
**************************************************************/
void PCB_relay1_on(void)
{
	GpioDataRegs.GPASET.bit.GPIO29 = 1;
}

/**************************************************************
* Funckija, ki izklopi Rele1 (Supply_main_relay)
**************************************************************/
void PCB_relay1_off(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
}

/**************************************************************
* Funckija, ki vklopi Rele2 (Supply_resistor_relay)
**************************************************************/
void PCB_relay2_on(void)
{
	GpioDataRegs.GPASET.bit.GPIO23 = 1;
}

/**************************************************************
* Funckija, ki izklopi Rele2 (Supply_resistor_relay)
**************************************************************/
void PCB_relay2_off(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO23 = 1;
}

/**************************************************************
* Funckija, ki vklopi Rele3 (Filter_main_relay)
**************************************************************/
void PCB_relay3_on(void)
{
	GpioDataRegs.GPASET.bit.GPIO19 = 1;
}

/**************************************************************
* Funckija, ki izklopi Rele3 (Filter_main_relay)
**************************************************************/
void PCB_relay3_off(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;
}
/*************************************************************/

/**************************************************************
* Funkcije za vklop/izklop LED
***************************************************************
* Funckija, ki vklopi LED FAULT (SIG_FAULT)
**************************************************************/
void PCB_LED_FAULT_on(void)
{
	GpioDataRegs.GPBSET.bit.GPIO40 = 1;
}

/**************************************************************
* Funckija, ki izklopi LED FAULT (SIG_FAULT)
**************************************************************/
void PCB_LED_FAULT_off(void)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO40 = 1;
}

/**************************************************************
* Funckija, ki vklopi LED READY (SIG_READY)
**************************************************************/
void PCB_LED_READY_on(void)
{
	GpioDataRegs.GPBSET.bit.GPIO50 = 1;
}

/**************************************************************
* Funckija, ki izklopi LED READY (SIG_READY)
**************************************************************/
void PCB_LED_READY_off(void)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;
}

/**************************************************************
* Funckija, ki vklopi LED WORKING (SIG_WORKING)
**************************************************************/
void PCB_LED_WORKING_on(void)
{
	GpioDataRegs.GPASET.bit.GPIO15 = 1;
}

/**************************************************************
* Funckija, ki izklopi LED WORKING (SIG_WORKING)
**************************************************************/
void PCB_LED_WORKING_off(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO15 = 1;
}
/*************************************************************/

/**************************************************************
* Funkcije za stanje tipk
***************************************************************
* Funckija ki vrne stanje ENABLE tipke (SW_ENABLE)
**************************************************************/
bool PCB_SW_ENABLE(void)
{
	if (GpioDataRegs.GPBDAT.bit.GPIO51 == 1)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

/**************************************************************
* Funckija ki vrne stanje RESET tipke (SW_RESET)
**************************************************************/
bool PCB_SW_RESET(void)
{
	if (GpioDataRegs.GPADAT.bit.GPIO16 == 1)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}
/*************************************************************/


/**************************************************************
* Funckija ki prizge LED diodo
**************************************************************/
#pragma CODE_SECTION(PCB_LEDcard_on, "ramfuncs");
void PCB_LEDcard_on(void)
{
	GpioDataRegs.GPCSET.bit.GPIO83 = 1;
}

/**************************************************************
* Funckija ki ugasne LED diodo
**************************************************************/
#pragma CODE_SECTION(PCB_LEDcard_off, "ramfuncs");
void PCB_LEDcard_off(void)
{
	GpioDataRegs.GPCCLEAR.bit.GPIO83 = 1;
}

/**************************************************************
* Funckija ki spremeni stanje LED diode
**************************************************************/
#pragma CODE_SECTION(PCB_LEDcard_toggle, "ramfuncs");
void PCB_LEDcard_toggle(void)
{
	GpioDataRegs.GPCTOGGLE.bit.GPIO83 = 1;
}



/**************************************************************
* Funckija ki inicializira PCB
**************************************************************/
void PCB_init(void)
{
    EALLOW;

    /* IZHODI */
    // LED na card-u
    GPIO_SetupPinMux(83, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(83, GPIO_OUTPUT, GPIO_PUSHPULL);


    EDIS;

}
