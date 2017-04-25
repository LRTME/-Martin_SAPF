/************************************************************** 
* FILE:         FB_drv.c
* DESCRIPTION:  A/D in PWM driver for TMS320F2808
* AUTHOR:       Mitja Nemec
* START DATE:   19.8.2010
* VERSION:      1.0
*
* CHANGES : 
* VERSION   DATE        WHO                 DETAIL 
* 1.0       21.12.2009  Mitja Nemec         Initial version
*
****************************************************************/
#include "FB_bipolar.h"

/*
 * FB1 - vhodni mostic
 * FB2 - izhodni mostic
 */

// status ali delujemo ali ne
enum FB_STATE fb1_status = FB_DIS;
enum FB_STATE fb2_status = FB_DIS;

/**************************************************************
* nastavi mrtvi cas  
**************************************************************/
void FB1_dead_time(float dead_time)
{
    float cpu_cycle_time = (1.0/CPU_FREQ);

    long cycle_number;

    // naracunam koliko ciklov to znese
    cycle_number = dead_time/cpu_cycle_time;

    FB1_MODUL1.DBFED.bit.DBFED = cycle_number;
    FB1_MODUL1.DBRED.bit.DBRED = cycle_number;
    FB1_MODUL2.DBFED.bit.DBFED = cycle_number;
    FB1_MODUL2.DBRED.bit.DBRED = cycle_number;
}

void FB2_dead_time(float dead_time)
{
    float cpu_cycle_time = (1.0/CPU_FREQ);
    
    long cycle_number;
    
    // naracunam koliko ciklov to znese
    cycle_number = dead_time/cpu_cycle_time;

    FB2_MODUL1.DBFED.bit.DBFED = cycle_number;
    FB2_MODUL1.DBRED.bit.DBRED = cycle_number;
    FB2_MODUL2.DBFED.bit.DBFED = cycle_number;
    FB2_MODUL2.DBRED.bit.DBRED = cycle_number;
}

/**************************************************************
* Izklopi zgornja tranzistorja in vklopi spodnja, tako da je mostic kratkosticen
* in lahko napolneta bootstrap kondenzatorja
* returns:  
**************************************************************/
void FB1_bootstrap(void)
{
    FB1_MODUL1.AQCSFRC.bit.CSFA = 1;
    FB1_MODUL1.AQCSFRC.bit.CSFB = 1;

    FB1_MODUL2.AQCSFRC.bit.CSFA = 1;
    FB1_MODUL2.AQCSFRC.bit.CSFB = 1;

    fb1_status = FB_BOOTSTRAP;
}

void FB2_bootstrap(void)
{
    FB2_MODUL1.AQCSFRC.bit.CSFA = 1;
    FB2_MODUL1.AQCSFRC.bit.CSFB = 1;

    FB2_MODUL2.AQCSFRC.bit.CSFA = 1;
    FB2_MODUL2.AQCSFRC.bit.CSFB = 1;

    fb2_status = FB_BOOTSTRAP;
}

/**************************************************************
* Izklopi vse tranzistorje - urgentno
* returns:
**************************************************************/
void FB1_trip(void)
{
    FB1_MODUL1.TZFRC.bit.OST = 1;
    FB1_MODUL2.TZFRC.bit.OST = 1;

    fb1_status = FB_TRIP;
}

void FB2_trip(void)
{
    FB2_MODUL1.TZFRC.bit.OST = 1;
    FB2_MODUL2.TZFRC.bit.OST = 1;

    fb2_status = FB_TRIP;
}

/**************************************************************
* Izklopi vse tranzistorje
* returns:  
**************************************************************/
void FB1_disable(void)
{
    FB1_MODUL1.AQCSFRC.bit.CSFA = 1;
    FB1_MODUL1.AQCSFRC.bit.CSFB = 2;

    FB1_MODUL2.AQCSFRC.bit.CSFA = 1;
    FB1_MODUL2.AQCSFRC.bit.CSFB = 2;

    fb1_status = FB_DIS;
}

void FB2_disable(void)
{
	FB2_MODUL1.AQCSFRC.bit.CSFA = 1;
    FB2_MODUL1.AQCSFRC.bit.CSFB = 2;

    FB2_MODUL2.AQCSFRC.bit.CSFA = 1;
    FB2_MODUL2.AQCSFRC.bit.CSFB = 2;

    fb2_status = FB_DIS;
}

/**************************************************************
* vklopi vse izhode
* returns:  
**************************************************************/
void FB1_enable(void)
{
	// ce je bil Trip aktiviran, ga resetiram
	EALLOW;
	FB1_MODUL1.TZCLR.bit.OST = 1;
	FB1_MODUL2.TZCLR.bit.OST = 1;
	EDIS;

    FB1_MODUL1.AQCSFRC.bit.CSFA = 0;
    FB1_MODUL1.AQCSFRC.bit.CSFB = 0;

    FB1_MODUL2.AQCSFRC.bit.CSFA = 0;
    FB1_MODUL2.AQCSFRC.bit.CSFB = 0;

    fb1_status = FB_EN;
}

void FB2_enable(void)
{
	// ce je bil Trip aktiviran, ga resetiram
	EALLOW;
	FB2_MODUL1.TZCLR.bit.OST = 1;
	FB2_MODUL2.TZCLR.bit.OST = 1;
	EDIS;

	// potem pa nastavim AQ
	FB2_MODUL1.AQCSFRC.bit.CSFA = 0;
    FB2_MODUL1.AQCSFRC.bit.CSFB = 0;

    FB2_MODUL2.AQCSFRC.bit.CSFA = 0;
    FB2_MODUL2.AQCSFRC.bit.CSFB = 0;

    // in signaliziram stanje
    fb2_status = FB_EN;
}

/**************************************************************
* vrne status (delam/ne delam)
* returns:  
**************************************************************/
int FB1_status(void)
{
    return(fb1_status);
}

int FB2_status(void)
{
    return(fb2_status);
}

/**************************************************************
* Funkcija, ki popiše registre za PWM1,2 in 3. Znotraj funkcije
* se omogoèi interrupt za proženje ADC, popiše se perioda, compare
* register, tripzone register, omogoèi se izhode za PWM...
* return:void
**************************************************************/
void FB1_init(void)
{
    // setup timer base 
    FB1_MODUL1.TBPRD = FB_PERIOD;       //nastavljeno na 25us, PWM_PERIOD = 50us
    FB1_MODUL2.TBPRD = FB_PERIOD;
    // init timer
    FB1_MODUL1.TBCTL.bit.PHSDIR = 0;       // count up after sync
    FB1_MODUL1.TBCTL.bit.CLKDIV = 0;
    FB1_MODUL1.TBCTL.bit.HSPCLKDIV = 0;
    FB1_MODUL1.TBCTL.bit.SYNCOSEL = 1;     // sync out on zero
    FB1_MODUL1.TBCTL.bit.PRDLD = 0;        // shadowed period reload on zero
    FB1_MODUL1.TBCTL.bit.PHSEN = 0;        // master timer does not sync

    FB1_MODUL2.TBCTL.bit.PHSDIR = 0;       // count up after sync
    FB1_MODUL2.TBCTL.bit.CLKDIV = 0;
    FB1_MODUL2.TBCTL.bit.HSPCLKDIV = 0;
    FB1_MODUL2.TBCTL.bit.SYNCOSEL = 1;     // sync out on zero
    FB1_MODUL2.TBCTL.bit.PRDLD = 0;        // shadowed period reload
    FB1_MODUL2.TBCTL.bit.PHSEN = 0;        // master timer does not sync
    
    // debug mode behaviour
    #if FB1_DEBUG == 0
    FB1_MODUL1.TBCTL.bit.FREE_SOFT = 0;  // stop after current cycle
    FB1_MODUL2.TBCTL.bit.FREE_SOFT = 0;  // stop after current cycle
    #endif
    #if FB1_DEBUG == 1
    FB1_MODUL1.TBCTL.bit.FREE_SOFT = 1;  // stop after current cycle
    FB1_MODUL2.TBCTL.bit.FREE_SOFT = 1;  // stop after current cycle
    #endif
    #if FB1_DEBUG == 2
    FB1_MODUL1.TBCTL.bit.FREE_SOFT = 3;  // run free
    FB1_MODUL2.TBCTL.bit.FREE_SOFT = 3;  // run free
    #endif

    // Init Timer-Base Phase Register for EPWM1-EPWM2
    FB1_MODUL1.TBPHS.bit.TBPHS = 0;
    FB1_MODUL2.TBPHS.bit.TBPHS = 0;

    // compare setup
    // Init Compare Control Register for EPWM1-EPWM2
    FB1_MODUL1.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // re-load on zero
    FB1_MODUL1.CMPCTL.bit.SHDWAMODE = CC_SHADOW; // shadowed compare reload

    FB1_MODUL2.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // re-load on zero
    FB1_MODUL2.CMPCTL.bit.SHDWAMODE = CC_SHADOW; // shadowed compare reload

    FB1_MODUL1.CMPA.bit.CMPA = FB_PERIOD/4; //50% duty cycle
    FB1_MODUL2.CMPA.bit.CMPA = FB_PERIOD/4; //50% duty cycle

    // Init Action Qualifier Output A Register 
    FB1_MODUL1.AQSFRC.bit.RLDCSF = 0;            // load AQ on zero
    FB1_MODUL2.AQSFRC.bit.RLDCSF = 0;            // load AQ on zero

    FB1_MODUL1.AQCTLA.bit.CAU = AQ_SET;      // set output on CMPA_UP
    FB1_MODUL1.AQCTLA.bit.CAD = AQ_CLEAR;    // clear output on CMPA_DOWN
    FB1_MODUL1.AQCTLB.bit.CAU = AQ_SET;      // set output on CMPA_UP
    FB1_MODUL1.AQCTLB.bit.CAD = AQ_CLEAR;    // clear output on CMPA_DOWN
    
    FB1_MODUL2.AQCTLA.bit.CAU = AQ_CLEAR;    // clear output on CMPA_UP
    FB1_MODUL2.AQCTLA.bit.CAD = AQ_SET;      // set output on CMPA_DOWN
    FB1_MODUL2.AQCTLB.bit.CAU = AQ_CLEAR;      // set output on CMPA_UP
    FB1_MODUL2.AQCTLB.bit.CAD = AQ_SET;    // clear output on CMPA_DOWN

    // Dead Time
    FB1_MODUL1.DBCTL.bit.IN_MODE = 2;    //
    FB1_MODUL1.DBCTL.bit.POLSEL = 2;     // active high complementary mode
    FB1_MODUL1.DBCTL.bit.OUT_MODE = 3;   // dead band on both outputs

    FB1_MODUL2.DBCTL.bit.IN_MODE = 2;    //
    FB1_MODUL2.DBCTL.bit.POLSEL = 2;     // active high complementary mode
    FB1_MODUL2.DBCTL.bit.OUT_MODE = 3;   // dead band on both outputs

    FB1_MODUL1.DBFED.bit.DBFED = FB_DEAD_TIME;
    FB1_MODUL1.DBRED.bit.DBRED = FB_DEAD_TIME;
    FB1_MODUL2.DBFED.bit.DBFED = FB_DEAD_TIME;
    FB1_MODUL2.DBRED.bit.DBRED = FB_DEAD_TIME;

    // trip zone functionality
    EALLOW;
    FB1_MODUL1.TZSEL.bit.OSHT1 = 1;      // select which input triggers tripzone (TZ1)
    FB1_MODUL1.TZCTL.bit.TZA = 2;        // force low
    FB1_MODUL1.TZCTL.bit.TZB = 2;        // force low
    FB1_MODUL1.TZCLR.bit.OST = 1;        // clear any pending flags

    FB1_MODUL2.TZSEL.bit.OSHT1 = 1;      // select which input triggers tripzone (TZ1)
    FB1_MODUL2.TZCTL.bit.TZA = 2;        // force low
    FB1_MODUL2.TZCTL.bit.TZB = 2;        // force low
    FB1_MODUL2.TZCLR.bit.OST = 1;        // clear any pending flags
    EDIS;

    // disable outputs by default
    FB1_disable();

    // output pin setup
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;		// ePWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;		// ePWM1B
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;		// ePWM2A
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;		// ePWM2B
    EDIS;
    
}
void FB2_init(void)
{
    // setup timer base
    FB2_MODUL1.TBPRD = FB_PERIOD;       //nastavljeno na 25us, PWM_PERIOD = 50us
    FB2_MODUL2.TBPRD = FB_PERIOD;
    // init timer
    FB2_MODUL1.TBCTL.bit.PHSDIR = 0;       // count up after sync
    FB2_MODUL1.TBCTL.bit.CLKDIV = 0;
    FB2_MODUL1.TBCTL.bit.HSPCLKDIV = 0;
    FB2_MODUL1.TBCTL.bit.SYNCOSEL = 1;     // sync out on zero
    FB2_MODUL1.TBCTL.bit.PRDLD = 0;        // shadowed period reload on zero
    FB2_MODUL1.TBCTL.bit.PHSEN = 0;        // master timer does not sync

    FB2_MODUL2.TBCTL.bit.PHSDIR = 0;       // count up after sync
    FB2_MODUL2.TBCTL.bit.CLKDIV = 0;
    FB2_MODUL2.TBCTL.bit.HSPCLKDIV = 0;
    FB2_MODUL2.TBCTL.bit.SYNCOSEL = 1;     // sync out on zero
    FB2_MODUL2.TBCTL.bit.PRDLD = 0;        // shadowed period reload
    FB2_MODUL2.TBCTL.bit.PHSEN = 0;        // master timer does not sync

    // debug mode behaviour
    #if FB2_DEBUG == 0
    FB2_MODUL1.TBCTL.bit.FREE_SOFT = 0;  // stop after current cycle
    FB2_MODUL2.TBCTL.bit.FREE_SOFT = 0;  // stop after current cycle
    #endif
    #if FB2_DEBUG == 1
    FB2_MODUL1.TBCTL.bit.FREE_SOFT = 1;  // stop after current cycle
    FB2_MODUL2.TBCTL.bit.FREE_SOFT = 1;  // stop after current cycle
    #endif
    #if FB2_DEBUG == 2
    FB2_MODUL1.TBCTL.bit.FREE_SOFT = 3;  // run free
    FB2_MODUL2.TBCTL.bit.FREE_SOFT = 3;  // run free
    #endif

    // Init Timer-Base Phase Register for EPWM1-EPWM2
    FB2_MODUL1.TBPHS.bit.TBPHS = 0;
    FB2_MODUL2.TBPHS.bit.TBPHS = 0;

    // compare setup
    // Init Compare Control Register for EPWM1-EPWM2
    FB2_MODUL1.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // re-load on zero
    FB2_MODUL1.CMPCTL.bit.SHDWAMODE = CC_SHADOW; // shadowed compare reload

    FB2_MODUL2.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // re-load on zero
    FB2_MODUL2.CMPCTL.bit.SHDWAMODE = CC_SHADOW; // shadowed compare reload

    FB2_MODUL1.CMPA.bit.CMPA = FB_PERIOD/4; //50% duty cycle
    FB2_MODUL2.CMPA.bit.CMPA = FB_PERIOD/4; //50% duty cycle

    // Init Action Qualifier Output A Register
    FB2_MODUL1.AQSFRC.bit.RLDCSF = 0;            // load AQ on zero
    FB2_MODUL2.AQSFRC.bit.RLDCSF = 0;            // load AQ on zero

    FB2_MODUL1.AQCTLA.bit.CAU = AQ_SET;      // set output on CMPA_UP
    FB2_MODUL1.AQCTLA.bit.CAD = AQ_CLEAR;    // clear output on CMPA_DOWN
    FB2_MODUL1.AQCTLB.bit.CAU = AQ_SET;      // set output on CMPA_UP
    FB2_MODUL1.AQCTLB.bit.CAD = AQ_CLEAR;    // clear output on CMPA_DOWN

    FB2_MODUL2.AQCTLA.bit.CAU = AQ_CLEAR;    // clear output on CMPA_UP
    FB2_MODUL2.AQCTLA.bit.CAD = AQ_SET;      // set output on CMPA_DOWN
    FB2_MODUL2.AQCTLB.bit.CAU = AQ_CLEAR;      // set output on CMPA_UP
    FB2_MODUL2.AQCTLB.bit.CAD = AQ_SET;    // clear output on CMPA_DOWN

    // Dead Time
    FB2_MODUL1.DBCTL.bit.IN_MODE = 2;    //
    FB2_MODUL1.DBCTL.bit.POLSEL = 2;     // active high complementary mode
    FB2_MODUL1.DBCTL.bit.OUT_MODE = 3;   // dead band on both outputs

    FB2_MODUL2.DBCTL.bit.IN_MODE = 2;    //
    FB2_MODUL2.DBCTL.bit.POLSEL = 2;     // active high complementary mode
    FB2_MODUL2.DBCTL.bit.OUT_MODE = 3;   // dead band on both outputs

    FB2_MODUL1.DBFED.bit.DBFED = FB_DEAD_TIME;
    FB2_MODUL1.DBRED.bit.DBRED = FB_DEAD_TIME;
    FB2_MODUL2.DBFED.bit.DBFED = FB_DEAD_TIME;
    FB2_MODUL2.DBRED.bit.DBRED = FB_DEAD_TIME;

    // trip zone functionality
    EALLOW;
    FB2_MODUL1.TZSEL.bit.OSHT1 = 1;      // select which input triggers tripzone
    FB2_MODUL1.TZCTL.bit.TZA = 2;        // force low
    FB2_MODUL1.TZCTL.bit.TZB = 2;        // force low
    FB2_MODUL1.TZCLR.bit.OST = 1;        // clear any pending flags

    FB2_MODUL2.TZSEL.bit.OSHT1 = 1;      // select which input triggers tripzone
    FB2_MODUL2.TZCTL.bit.TZA = 2;        // force low
    FB2_MODUL2.TZCTL.bit.TZB = 2;        // force low
    FB2_MODUL2.TZCLR.bit.OST = 1;        // clear any pending flags
    EDIS;

    // disable outputs by default
    FB2_disable();

    // output pin setup
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;		// ePWM3A
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;		// ePWM3B
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;		// ePWM4A
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;		// ePWM4B
    EDIS;

}
//end of FB_PWM_init

/**************************************************************
* Funkcija, ki nastavi periodo, za doseganje zeljene periode
* in je natancna na cikel natanèno
* return: void
* arg1: zelena perioda
**************************************************************/
#pragma CODE_SECTION(FB1_period, "ramfuncs");
void FB1_period(float perioda)
{
    // spremenljivke
    float   temp_tbper;
    static float ostanek = 0;
    long celi_del;

    // naracunam TBPER (CPU_FREQ * perioda)
    temp_tbper = perioda * CPU_FREQ/2;

    // izlocim celi del in ostanek
    celi_del = (long)temp_tbper;
    ostanek = temp_tbper - celi_del;
    // povecam celi del, ce je ostanek veji od 1
    if (ostanek > 1.0)
    {
        ostanek = ostanek - 1.0;
        celi_del = celi_del + 1;
    }
    // nastavim TBPER
    FB1_MODUL1.TBPRD = celi_del;
    FB1_MODUL2.TBPRD = celi_del;
}

#pragma CODE_SECTION(FB2_period, "ramfuncs");
void FB2_period(float perioda)
{
    // spremenljivke
    float   temp_tbper;
    static float ostanek = 0;
    long celi_del;

    // naracunam TBPER (CPU_FREQ * perioda)
    temp_tbper = perioda * CPU_FREQ/2;
    
    // izlocim celi del in ostanek
    celi_del = (long)temp_tbper;
    ostanek = temp_tbper - celi_del;
    // povecam celi del, ce je ostanek veji od 1
    if (ostanek > 1.0)
    {
        ostanek = ostanek - 1.0;
        celi_del = celi_del + 1;
    }
    // nastavim TBPER
    FB2_MODUL1.TBPRD = celi_del;
    FB2_MODUL2.TBPRD = celi_del;
}//end of FB_period

/**************************************************************
* Funkcija, ki nastavi periodo, za doseganje zeljene frekvence
* in je natancna na cikel natanèno
* return: void
* arg1: zelena frekvenca
**************************************************************/
#pragma CODE_SECTION(FB1_frequency, "ramfuncs");
void FB1_frequency(float frekvenca)
{
    // spremenljivke
    float   temp_tbper;
    static float ostanek = 0;
    long celi_del;

    // naracunam TBPER (CPU_FREQ / SAMPLING_FREQ) - 1
    temp_tbper = ((CPU_FREQ/2)/2)/frekvenca;

    // izlocim celi del in ostanek
    celi_del = (long)temp_tbper;
    ostanek = temp_tbper - celi_del;
    // povecam celi del, ce je ostanek veji od 1
    if (ostanek > 1.0)
    {
        ostanek = ostanek - 1.0;
        celi_del = celi_del + 1;
    }
    // nastavim TBPER
    FB1_MODUL1.TBPRD = celi_del - 1;
    FB1_MODUL2.TBPRD = celi_del - 1;
}

#pragma CODE_SECTION(FB2_frequency, "ramfuncs");
void FB2_frequency(float frekvenca)
{
    // spremenljivke
    float   temp_tbper;
    static float ostanek = 0;
    long celi_del;

    // naracunam TBPER (CPU_FREQ / SAMPLING_FREQ) - 1
    temp_tbper = ((CPU_FREQ/2)/2)/frekvenca;

    // izlocim celi del in ostanek
    celi_del = (long)temp_tbper;
    ostanek = temp_tbper - celi_del;
    // povecam celi del, ce je ostanek veji od 1
    if (ostanek > 1.0)
    {
        ostanek = ostanek - 1.0;
        celi_del = celi_del + 1;
    }
    // nastavim TBPER
    FB2_MODUL1.TBPRD = celi_del - 1;
    FB2_MODUL2.TBPRD = celi_del - 1;
}//end of FB_frequency

/**************************************************************
* Funkcija, ki na podlagi vklopnega razmerja in izbranega vektorja
* vklopi doloèene tranzistorje
* return: void
* arg1: vklopno razmerje od -1.0 do +1.0 (format IQ)
**************************************************************/
#pragma CODE_SECTION(FB1_update, "ramfuncs");
void FB1_update(float duty)
{
    unsigned int compare;

    unsigned int perioda;

    // zašèita za duty cycle
    //(zašèita za sektor je narejena v default switch case stavku)
    if (duty < (-1.0)) duty = (-1.0);
    if (duty > (+1.0)) duty = (+1.0);

    perioda = FB1_MODUL1.TBPRD;
    // koda da naracunam vrednost, ki bo sla v CMPR register

    compare = 0.5 * (perioda - perioda * duty);

    // vpisem v register
    FB1_MODUL1.CMPA.bit.CMPA = compare;
    FB1_MODUL2.CMPA.bit.CMPA = compare;

}
#pragma CODE_SECTION(FB2_update, "ramfuncs");
void FB2_update(float duty)
{
    unsigned int compare;

    unsigned int perioda;

    // zašèita za duty cycle
    //(zašèita za sektor je narejena v default switch case stavku)
    if (duty < (-1.0)) duty = (-1.0);
    if (duty > (+1.0)) duty = (+1.0);

    perioda = FB2_MODUL1.TBPRD;
    // koda da naracunam vrednost, ki bo sla v CMPR register

    compare = 0.5 * (perioda - perioda * duty);

    // vpisem v register
    FB2_MODUL1.CMPA.bit.CMPA = compare;
    FB2_MODUL2.CMPA.bit.CMPA = compare;

}//end of FB_update
  
/**************************************************************
* Funkcija, ki starta PWM èasovnike. Znotraj funkcije nastavimo
* naèin štetja èasovnikov
* return: void
**************************************************************/
void FB1_start(void)
{
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;

    FB1_MODUL1.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  //up-down count mode
    FB1_MODUL2.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  //up-down count mode

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

}
void FB2_start(void)
{
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    
    FB2_MODUL1.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  //up-down count mode
    FB2_MODUL2.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  //up-down count mode
    
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
    
}//end of FB_start
