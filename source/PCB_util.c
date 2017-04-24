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
* klice samo iz periodicne prekinitve - GPIO33
**************************************************************/
#pragma CODE_SECTION(PCB_WD_KICK_int, "ramfuncs");
void PCB_WD_KICK_int(void)
{
    // brcnemo Watch dog
    if (counter <= PCB_WD_KICK_ON)
    {
        GpioDataRegs.GPBSET.bit.GPIO33 = 1;
    }
    else
    {
        GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;
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
* Funckija, ki vrne stanje strojne zascite s CPLD-ja (trip)
* - GPIO19
**************************************************************/
bool PCB_CPLD_trip(void)
{
	if (GpioDataRegs.GPADAT.bit.GPIO19 == 1)
    {
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

/**************************************************************
* Funckija ki resetira zapah na CPLD (LATCH_RESET)
*
* !!!!!!!!!!!!!!!!!!!!!
* je ne smeš klicati iz prekinitve
* !!!!!!!!!!!!!!!!!!!!
*
**************************************************************/
#pragma CODE_SECTION(PCB_CPLD_LATCH_RESET, "ramfuncs");
void PCB_CPLD_LATCH_RESET(void)
{
    GpioDataRegs.GPASET.bit.GPIO27 = 1;
    DELAY_US(500L);
    GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;
}

/**************************************************************
* Funckija, ki vrne stanje "over_voltage" zapaha s CPLD-ja
* (over_voltage) - GPIO17
**************************************************************/
bool PCB_CPLD_over_voltage(void)
{
	if(GpioDataRegs.GPADAT.bit.GPIO17 == 1)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/**************************************************************
* Funckija, ki vrne stanje "over_current_supply" zapaha s CPLD-ja
* (over_current_supply) - GPIO11
**************************************************************/
bool PCB_CPLD_over_current_supply(void)
{
	if(GpioDataRegs.GPADAT.bit.GPIO11 == 1)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/**************************************************************
* Funckija, ki vrne stanje "over_current_filter" zapaha s CPLD-ja
* (over_current_filter) - GPIO15
**************************************************************/
bool PCB_CPLD_over_current_filter(void)
{
	if(GpioDataRegs.GPADAT.bit.GPIO15 == 1)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}
/**************************************************************
* Funkcije izhodov na CPLD
***************************************************************
* Funckija izhoda MOSFET_MCU - GPIO25
**************************************************************/
void PCB_CPLD_MOSFET_MCU_on(void)
{
	GpioDataRegs.GPASET.bit.GPIO25 = 1;
}

void PCB_CPLD_MOSFET_MCU_off(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO25 = 1;
}

/**************************************************************
* Funkcije za vklop/izklop relejev (preko CPLD)
***************************************************************
* Funckija, ki vklopi Rele1 (Supply_main_relay) - GPIO29
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
* Funckija, ki vklopi Rele2 (Supply_resistor_relay) - GPIO23
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
* Funckija, ki vklopi Rele3 (Filter_main_relay) - GPIO72
**************************************************************/
void PCB_relay3_on(void)
{
	GpioDataRegs.GPCSET.bit.GPIO72 = 1;
}

/**************************************************************
* Funckija, ki izklopi Rele3 (Filter_main_relay)
**************************************************************/
void PCB_relay3_off(void)
{
	GpioDataRegs.GPCCLEAR.bit.GPIO72 = 1;
}
/*************************************************************/

/**************************************************************
* Funkcije za vklop/izklop LED
***************************************************************
* Funckija, ki vklopi LED FAULT (SIG_FAULT)
**************************************************************/
void PCB_LED_FAULT_on(void)
{
	GpioDataRegs.GPASET.bit.GPIO12 = 1;
}

/**************************************************************
* Funckija, ki izklopi LED FAULT (SIG_FAULT)
**************************************************************/
void PCB_LED_FAULT_off(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;
}

/**************************************************************
* Funckija, ki vklopi LED READY (SIG_READY)
**************************************************************/
void PCB_LED_READY_on(void)
{
	GpioDataRegs.GPBSET.bit.GPIO38 = 1;
}

/**************************************************************
* Funckija, ki izklopi LED READY (SIG_READY)
**************************************************************/
void PCB_LED_READY_off(void)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO38 = 1;
}

/**************************************************************
* Funckija, ki vklopi LED WORKING (SIG_WORKING)
**************************************************************/
void PCB_LED_WORKING_on(void)
{
	GpioDataRegs.GPASET.bit.GPIO18 = 1;
}

/**************************************************************
* Funckija, ki izklopi LED WORKING (SIG_WORKING)
**************************************************************/
void PCB_LED_WORKING_off(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;
}
/*************************************************************/

/**************************************************************
* Funkcije za stanje tipk
***************************************************************
* Funckija ki vrne stanje ENABLE tipke (SW_ENABLE)
**************************************************************/
bool PCB_SW_ENABLE(void)
{
	if (GpioDataRegs.GPBDAT.bit.GPIO39 == 1)
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
	if (GpioDataRegs.GPCDAT.bit.GPIO69 == 1)
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
/**************************************************************/
    /* IZHODI */
    	// TODO

        //Releji
        // GPIO29 - Supply_main_relay (Relay 1)
        GPIO_SetupPinMux(29, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(29, GPIO_OUTPUT, GPIO_PUSHPULL);

        // GPIO23 - Supply_resistor_relay (Relay 2)
        GPIO_SetupPinMux(23, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(23, GPIO_OUTPUT, GPIO_PUSHPULL);

        // GPIO72 - Filter_main_relay (Relay 3)
        GPIO_SetupPinMux(72, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(72, GPIO_OUTPUT, GPIO_PUSHPULL);

    	//LEDice
        // GPIO12 - LED_FAULT
        GPIO_SetupPinMux(12, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(12, GPIO_OUTPUT, GPIO_PUSHPULL);

        // GPIO38 - LED_READY
        GPIO_SetupPinMux(38, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(38, GPIO_OUTPUT, GPIO_PUSHPULL);

        // GPIO18 - LED_WORKING
        GPIO_SetupPinMux(18, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(18, GPIO_OUTPUT, GPIO_PUSHPULL);

        // LED na card-u
        GPIO_SetupPinMux(83, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(83, GPIO_OUTPUT, GPIO_PUSHPULL);

        //WD kick
        // GPIO33 - WD kick
        GPIO_SetupPinMux(33, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(33, GPIO_OUTPUT, GPIO_PUSHPULL);

        //CPLD
        // GPIO25 - MOSFET_MCU
        GPIO_SetupPinMux(25, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(25, GPIO_OUTPUT, GPIO_PUSHPULL);

        // GPIO27 - LATCH_RESET
        GPIO_SetupPinMux(27, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(27, GPIO_OUTPUT, GPIO_PUSHPULL);
		
		// GPIO17 - over_voltage
		GPIO_SetupPinMux(17, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(17, GPIO_OUTPUT, GPIO_PUSHPULL);
		
		// GPIO11 - over_current_supply
		GPIO_SetupPinMux(11, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(11, GPIO_OUTPUT, GPIO_PUSHPULL);
		
		// GPIO15 - over_current_filter
		GPIO_SetupPinMux(15, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(15, GPIO_OUTPUT, GPIO_PUSHPULL);

/***************************************************************/

        /* VHODI */
        //Tipke
        // GPIO39 - SW_ENABLE
        GPIO_SetupPinMux(39, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(39, GPIO_INPUT, GPIO_INPUT);

        // GPIO26 - SW_RESET
        GPIO_SetupPinMux(69, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(69, GPIO_INPUT, GPIO_INPUT);

        //CPLD
		// GPIO19 - trip
		GPIO_SetupPinMux(19, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(19, GPIO_INPUT, GPIO_INPUT);

        // GPIO17 - over_voltage
        GPIO_SetupPinMux(17, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(17, GPIO_INPUT, GPIO_INPUT);

        // GPIO11 - over_current_supply
        GPIO_SetupPinMux(11, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(11, GPIO_INPUT, GPIO_INPUT);

        // GPIO15 - over_current_filter
        GPIO_SetupPinMux(15, GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(15, GPIO_INPUT, GPIO_INPUT);
/***************************************************************/

        // postavim v privzeto stanje
        PCB_relay1_off();
        PCB_relay2_off();
        PCB_relay3_off();

        PCB_LED_FAULT_off();
		PCB_LED_READY_off();
		PCB_LED_WORKING_off();
}
