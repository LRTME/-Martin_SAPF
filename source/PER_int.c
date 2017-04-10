/****************************************************************
* FILENAME:     PER_int.c
* DESCRIPTION:  periodic interrupt code
* AUTHOR:       Mitja Nemec
*
****************************************************************/
#include    "PER_int.h"
#include    "TIC_toc.h"

// za oceno obremenjenosti CPU-ja
float   cpu_load  = 0.0;
long    interrupt_cycles = 0;

// temperatura procesorja
float	cpu_temp = 0.0;
float	napetost = 0.0;

// tokovi
float   IS = 0.0;
float   IF = 0.0;

float   IS_offset = 2048;
float   IF_offset = 2048;

float   IS_gain = (5.0 / 0.625 ) * (7.5 / 6.2) * (3.3 / 4096);
float   IF_gain = (25.0 / 0.625 ) * (7.5 / 6.2) * (3.3 / 4096);

long    current_offset_counter = 0;

// napetosti
float   u_ac = 0.0;
float   u_dc = 0.0;
float   u_f = 0.0;
float   u_out = 0.0;

float   u_ac_offset = 2048.0;
float   u_dc_offset = 2048.0;
float   u_f_offset = 2048.0;
float   u_out_offset = 2048.0;

float   u_ac_gain = ((1000 + 0.47) / (5 * 0.47)) * U_AC_CORR_F * (3.3 / 4096);
float   u_dc_gain = ((330 + 1.8) / (5 * 1.8)) * DEL_UDC_CORR_F * (3.3 / 4096);
float   u_f_gain = ((200 + 1.8) / (5 * 1.8)) * (3.3 / 4096);
float   u_out_gain = ((1000 + 0.47) / (5 * 0.47)) * U_OUT_CORR_F * (3.3 / 4096);

// NTC
float beta_NTC = 3988;
float R_NTC25 = 5000;
float T_NTC25 = 298;
float V_NTC = 0.0;
float R_NTC = 0.0;
float T_NTC = 0.0;

// filtrirana napetost DC linka
DC_float    u_dc_f = DC_FLOAT_DEFAULTS;
float   u_dc_filtered = 0.0;

// prvi harmonik in RMS vhodne omrežne napetosti (u_ac)
DFT_float   u_ac_dft = DFT_FLOAT_DEFAULTS;
float   u_ac_rms = 0.0;
float   u_ac_form = 0.0;

// prvi harmonik in RMS izhodne napetosti (u_out)
DFT_float   u_out_dft = DFT_FLOAT_DEFAULTS;
float   u_out_rms = 0.0;
float   u_out_form = 0.0;

// regulacija napetosti enosmernega tokokroga
PID_float   u_dc_reg = PID_FLOAT_DEFAULTS;
SLEW_float  u_dc_slew = SLEW_FLOAT_DEFAULTS;

// regulacija omreznega toka
PID_float   IS_reg = PID_FLOAT_DEFAULTS;

// sinhronizacija na omrežje
float       sync_base_freq = SWITCH_FREQ;
PID_float   sync_reg    = PID_FLOAT_DEFAULTS;
float       sync_switch_freq = SWITCH_FREQ;
float       sync_grid_freq = ((SWITCH_FREQ/SAMPLING_RATIO)/SAMPLE_POINTS);
bool        sync_use = TRUE;

// samo za statistiko meritev
STAT_float  statistika = STAT_FLOAT_DEFAULTS;

// za oceno DC-link toka
ABF_float   I_cap_dc = ABF_FLOAT_DEFAULTS;
float       I_dc_abf = 0.0;

// za zakasnitev omreznega toka
DELAY_float IS_delay = DELAY_FLOAT_DEFAULTS;

// filtriranje izhoda ocene
DC_float    I_dc_f = DC_FLOAT_DEFAULTS;

// filtriranje meritve
DC_float    IF_f = DC_FLOAT_DEFAULTS;
DC_float	IS_f = DC_FLOAT_DEFAULTS;

// vhodna moc
float	power_in = 0.0;

// izhodna moc
float   power_out = 0.0;

// temperatura hladilnika
float   temperatura = 0.0;

// prototipi funkcij
void get_electrical(void);
void sync(void);
void input_bridge_control(void);
void check_limits(void);
float NTC_temp(void);


// spremenljikva s katero štejemo kolikokrat se je prekinitev predolgo izvajala
int interrupt_overflow_counter = 0;

/**************************************************************
* Prekinitev, ki v kateri se izvaja regulacija
**************************************************************/
#pragma CODE_SECTION(PER_int, "ramfuncs");
void interrupt PER_int(void)
{
    /* lokalne spremenljivke */
    
    // najprej povem da sem se odzzval na prekinitev
    // Spustimo INT zastavico casovnika ePWM1
    EPwm1Regs.ETCLR.bit.INT = 1;
    // Spustimo INT zastavico v PIE enoti
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    
    // pozenem stoparico
    interrupt_cycles = TIC_time;
    TIC_start();

    // izracunam obremenjenost procesorja
    cpu_load = (float)interrupt_cycles / (CPU_FREQ/SAMPLE_FREQ);

    // povecam stevec prekinitev
    interrupt_cnt = interrupt_cnt + 1;
    if (interrupt_cnt >= SAMPLE_FREQ)
    {
        interrupt_cnt = 0;
    }

    // generiram zeljeno vrednost
    REF_GEN_update();

    // izracun napetosti, tokov in temperature hladilnika
    get_electrical();

    // preverim ali sem znotraj meja
    check_limits();

    // sinhronizacija na omrezje
    sync();

    // regulacija u_dc
    input_bridge_control();

    // spravim vrednosti v buffer za prikaz
    DLOG_GEN_update();
    
    /* preverim, èe me sluèajno èaka nova prekinitev.
       èe je temu tako, potem je nekaj hudo narobe
       saj je èas izvajanja prekinitve predolg
       vse skupaj se mora zgoditi najmanj 10krat,
       da reèemo da je to res problem
    */
    if (EPwm1Regs.ETFLG.bit.INT == TRUE)
    {
        // povecam stevec, ki steje take dogodke
        interrupt_overflow_counter = interrupt_overflow_counter + 1;
        
        // in ce se je vse skupaj zgodilo 10 krat se ustavim
        // v kolikor uC krmili kakšen resen HW, potem moèno
        // proporoèam lepše "hendlanje" takega dogodka
        // beri:ugasni moènostno stopnjo, ...
        if (interrupt_overflow_counter >= 10)
        {
        	// izklopim mostic
        	FB1_disable();
        	FB2_disable();

        	// izklopim vse kontaktorjev
        	PCB_relay1_off();
        	PCB_relay2_off();
        	PCB_relay3_off();

            asm(" ESTOP0");
        }
    }
    
    // stopam
    TIC_stop();
	PCB_WD_KICK_int();

}   // end of PWM_int

/**************************************************************
* Funckija, ki pripravi vse potrebno za izvajanje
* prekinitvene rutine
**************************************************************/
void PER_int_setup(void)
{
    // inicializiram data logger
    dlog.mode = Single;
    dlog.auto_time = 1;
    dlog.holdoff_time = 1;

    dlog.prescalar = 2;

    dlog.slope = Negative;
    dlog.trig = &ref_gen.kot;
    dlog.trig_value = 0.5;

    dlog.iptr1 = &u_ac;
    dlog.iptr2 = &IS;
    dlog.iptr3 = &IS_reg.Ref;
    dlog.iptr4 = &u_dc_filtered;
    dlog.iptr5 = &u_dc_reg.Ref;
    dlog.iptr6 = &u_out;
    dlog.iptr7 = &IS_reg.Err;
    dlog.iptr8 = &I_dc_abf;

    // inicializiram generator signalov
    ref_gen.type = REF_Step;
    ref_gen.amp = 1.0;
    ref_gen.offset = 0.0;
    ref_gen.freq = 10.0;
    ref_gen.duty = 0.5;
    ref_gen.slew = 100;
    ref_gen.samp_period = SAMPLE_TIME;

    // inicializiram DC filter
    DC_FLOAT_MACRO_INIT(u_dc_f);

    // inicilaliziram DFT
    DFT_FLOAT_MACRO_INIT(u_ac_dft);

    // inicializiram u_dc_slew
    u_dc_slew.In = DEL_UDC_REF;					// referenca u_dc = 40 V
    u_dc_slew.Slope_up = 10.0 * SAMPLE_TIME;	// 10 V/s
    u_dc_slew.Slope_down = u_dc_slew.Slope_up;

    // inicializiram regulator DC_link napetosti (empiricno doloceni parametri)
    u_dc_reg.Kp = 2.2;
    u_dc_reg.Ki = 0.0002;
    u_dc_reg.Kff = 0.8;
    u_dc_reg.OutMax = 4.9;		// IS_max 5.0 A
    u_dc_reg.OutMin = -4.9;		// IS_min -5.0 A

    // inicializiram regulator omreznega toka IS (Optimum iznosa)
    IS_reg.Kp = 0.1885;
    IS_reg.Ki = 0.004642;
    IS_reg.Kff = 1.0;
    IS_reg.OutMax = +0.99;    // zaradi bootstrap driverjev ne gre do 1.0
    IS_reg.OutMin = -0.99;    // zaradi bootstrap driverjev ne gre do 1.0

    // regulator frekvence
    sync_reg.Kp = 1000;
    sync_reg.Ki = 0.01;
    sync_reg.OutMax = +SWITCH_FREQ/10;
    sync_reg.OutMin = -SWITCH_FREQ/10;

    // inicializiram statistiko
    STAT_FLOAT_MACRO_INIT(statistika);

    // inicializiram ABF
                                    // 2000 Hz;     1000 Hz;     500 Hz,      100 Hz          50 Hz           10 Hz
    I_cap_dc.Alpha = 0.043935349;  	// 0.6911845;   0.394940272 ; 0.209807141; 0.043935349;    0.022091045;    0.004437948
    I_cap_dc.Beta = 0.00098696;   	// 0.394784176; 0.098696044; 0.024674011; 0.00098696;     0.00024674;     0.0000098696
    I_cap_dc.Capacitance = 5 * 0.0022;   	// 5 * 2200 uF

    // inicializiram delay_linijo
    DELAY_FLOAT_INIT(IS_delay)
    IS_delay.delay = 10;

    // inicializiram filter za oceno toka
    DC_FLOAT_MACRO_INIT(I_dc_f);

    // inicializiram filter za meritev toka
    DC_FLOAT_MACRO_INIT(IF_f);

    // inicializiram štoparico
    TIC_init();

    // Proženje prekinitve
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;    //sproži prekinitev na periodo
    EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;         //ob vsakem prvem dogodku
    EPwm1Regs.ETCLR.bit.INT = 1;                //clear possible flag
    EPwm1Regs.ETSEL.bit.INTEN = 1;              //enable interrupt

    // registriram prekinitveno rutino
    EALLOW;
    PieVectTable.EPWM1_INT = &PER_int;
    EDIS;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
    IER |= M_INT3;
    // da mi prekinitev teèe  tudi v real time naèinu
    // (za razhoršèevanje main zanke in BACK_loop zanke)
    SetDBGIER(M_INT3);
}

#pragma CODE_SECTION(get_electrical, "ramfuncs");
void get_electrical(void)
{
    static float   IS_offset_calib = 0;
    static float   IF_offset_calib = 0.0;

    // pocakam da ADC konca s pretvorbo
    ADC_wait();
    // poberem vrednosti iz AD pretvornika

    // kalibracija preostalega toka
    if (   (start_calibration == TRUE)
        && (calibration_done == FALSE))
    {
        // akumuliram offset
        IS_offset_calib = IS_offset_calib + IS_adc;
        IF_offset_calib = IF_offset_calib + IF_adc;

        // ko potece dovolj casa, sporocim da lahko grem naprej
        // in izracunam povprecni offset
        current_offset_counter = current_offset_counter + 1;
        if (current_offset_counter == (SAMPLE_FREQ * 1L))
        {
            calibration_done = TRUE;
            start_calibration = FALSE;
            IS_offset = IS_offset_calib / (SAMPLE_FREQ*1L);
            IF_offset = IF_offset_calib / (SAMPLE_FREQ*1L);

        }

        IS = 0.0;
        IF = 0.0;
        u_ac = 0.0;
        u_dc = 0.0;
        u_f = 0.0;
        u_out = 0.0;
    }
    else
    {
        IS = IS_gain * (IS_adc - IS_offset);
        IF = IF_gain * (IF_adc - IF_offset);
        u_ac = u_ac_gain * (u_ac_adc - u_ac_offset);
        u_dc = u_dc_gain * (DEL_UDC_adc - u_dc_offset);
        u_f = u_f_gain * (u_f_adc - u_f_offset);
        u_out = u_out_gain * (u_out_adc - u_out_offset);
    }

    // temperatura hladilnika
    temperatura = NTC_temp();

    // porcunam DFT napetosti
    // vhodna omrežna napetost - u_ac
    u_ac_dft.In = u_ac;
    DFT_FLOAT_MACRO(u_ac_dft);

    // izhodna napetost - u_out
    u_out_dft.In = u_out;
    DFT_FLOAT_MACRO(u_out_dft);

    // naraèunam amplitudo omrežne napetosti - u_ac
    u_ac_rms = ZSQRT2 * sqrt(u_ac_dft.SumA * u_ac_dft.SumA + u_ac_dft.SumB *u_ac_dft.SumB);

    // normiram, da dobim obliko
    u_ac_form = u_ac_dft.Out / (u_ac_rms * SQRT2);

    // naraèunam amplitudo izhodne napetosti - u_out
    u_out_rms = ZSQRT2 * sqrt(u_out_dft.SumA * u_out_dft.SumA + u_out_dft.SumB *u_out_dft.SumB);

    // normiram, da dobim obliko
    u_out_form = u_out_dft.Out / (u_out_rms * SQRT2);


    // filtriram DC link napetost
    u_dc_f.In = u_dc;
    DC_FLOAT_MACRO(u_dc_f);
    u_dc_filtered = u_dc_f.Mean;

    // izracunam kaksna moc je na izhodu in vhodu filtra
    power_out = u_f * IF;

    power_in = (24.0 / 230.0) * u_ac * IS;

    // zakasnim IS
    IS_delay.in = IS * IS_reg.Out;
    DELAY_FLOAT_CALC(IS_delay);

    // ocena dc toka z ABF
    I_cap_dc.u_cap_measured = u_dc;
    ABF_float_calc(&I_cap_dc);

    // se filtriram
    I_dc_f.In = -I_cap_dc.i_cap_estimated - IS_delay.out;
    DC_FLOAT_MACRO(I_dc_f);
    I_dc_abf = I_dc_f.Mean;

    // filtriram tudi meritev toka
    IF_f.In = IF;
    DC_FLOAT_MACRO(IF_f);


    // statistika
    statistika.In = u_dc;
    STAT_FLOAT_MACRO(statistika);
}

#pragma CODE_SECTION(NTC_temp, "ramfuncs");
float NTC_temp(void)
{
	V_NTC = M_TEMP_adc * (3.3/4096.0);
	R_NTC = 5100 * ((5/V_NTC) - 1);
	T_NTC = (T_NTC25 * beta_NTC)/(beta_NTC - T_NTC25 * log(R_NTC25/R_NTC)) - 273;

	return T_NTC;
}

#pragma CODE_SECTION(sync, "ramfuncs");
void sync(void)
{
	// sinhronizacija na omrezje
    sync_reg.Ref = 0;
    sync_reg.Fdb = u_ac_dft.SumA/u_ac_rms;
    PID_FLOAT_CALC(sync_reg);

    sync_switch_freq = sync_base_freq + sync_reg.Out;

    sync_grid_freq = ((sync_switch_freq/SAMPLING_RATIO)/SAMPLE_POINTS);

    if (sync_use == TRUE)
    {
        FB1_frequency(sync_switch_freq);
        FB2_frequency(sync_switch_freq);
    }
    else
    {
        FB1_frequency(sync_base_freq);
        FB2_frequency(sync_base_freq);
    }
}

#pragma CODE_SECTION(input_bridge_control, "ramfuncs");
void input_bridge_control(void)
{
    // regulacija deluje samo v teh primerih
    if (   (state == Standby)
        || (state == Enable)
        || (state == Working)
        || (state == Disable))
    {
        // najprej zapeljem zeljeno vrednost po rampi
        SLEW_FLOAT_CALC(u_dc_slew)

        // napetostni PI regulator
        u_dc_reg.Ref = u_dc_slew.Out;
        u_dc_reg.Fdb = u_dc_filtered;
        u_dc_reg.Ff = I_dc_abf * (230 / 24) * u_dc_filtered * SQRT2 / u_ac_rms;

        PID_FLOAT_CALC(u_dc_reg);

        // tokovni PI regulator s feed forward
        IS_reg.Ref = -u_dc_reg.Out * u_ac_form;
        IS_reg.Fdb = IS;
        IS_reg.Ff = (24.0 / 230.0) * u_ac/u_dc;
        PID_FLOAT_CALC(IS_reg);

        // posljem vse skupaj na mostic
        FB1_update(IS_reg.Out);
    }
    // sicer pa nicim integralna stanja
    else
    {
        u_dc_reg.Ui = 0.0;
        IS_reg.Ui = 0.0;
        FB1_update(0.0);
    }
}

#pragma CODE_SECTION(check_limits, "ramfuncs");
void check_limits(void)
{
    // samo èe je kalibracija konènana
    if (calibration_done == TRUE)
    {
         if (u_ac_rms > u_ac_RMS_MAX)
        {
            fault_flags.overvoltage_u_ac = TRUE;
            state = Fault_sensed;
            // izklopim mostic
            FB1_disable();
            FB2_disable();

            // izklopim vse kontaktorjev
            PCB_relay1_off();
            PCB_relay2_off();
            PCB_relay3_off();
        }

        if (   (u_ac_rms < u_ac_RMS_MIN)
                && (state != Initialization)
                && (state != Startup))
        {
            fault_flags.undervoltage_u_ac = TRUE;
            state = Fault_sensed;
            // izklopim mostic
            FB1_disable();
            FB2_disable();

            // izklopim vse kontaktorjev
            PCB_relay1_off();
            PCB_relay2_off();
            PCB_relay3_off();
        }

        if (u_dc > DEL_UDC_MAX)
        {
            fault_flags.overvoltage_u_dc = TRUE;
            state = Fault_sensed;
            // izklopim mostic
            FB1_disable();
            FB2_disable();

            // izklopim vse kontaktorjev
            PCB_relay1_off();
            PCB_relay2_off();
            PCB_relay3_off();
        }

        if (   (u_dc < DEL_UDC_MIN)
                && (state != Initialization)
                && (state != Startup))
        {
            fault_flags.undervoltage_u_dc = TRUE;
            state = Fault_sensed;
            // izklopim mostic
            FB1_disable();
            FB2_disable();

            // izklopim vse kontaktorjev
            PCB_relay1_off();
            PCB_relay2_off();
            PCB_relay3_off();
        }

        if ((IS > +IS_LIM) || (IS < -IS_LIM))
        {
        	fault_flags.overcurrent_IS = TRUE;
        	state = Fault_sensed;
        	// izklopim mostic
        	FB1_disable();
        	FB2_disable();

       		// izklopim vse kontaktorjev
       		PCB_relay1_off();
       		PCB_relay2_off();
       		PCB_relay3_off();
        }

        if ((IF > +IF_LIM) || (IF < -IF_LIM))
        {
            fault_flags.overcurrent_IF = TRUE;
            state = Fault_sensed;
            // izklopim mostic
            FB1_disable();
            FB2_disable();

            // izklopim vse kontaktorjev
            PCB_relay1_off();
            PCB_relay2_off();
            PCB_relay3_off();
        }

        if (PCB_CPLD_trip() == FALSE)
        {
        	fault_flags.HW_trip = TRUE;
        	state = Fault_sensed;
        	// izklopim mostic
        	FB1_disable();
        	FB2_disable();

        	// izklopim vse kontaktorjev
        	PCB_relay1_off();
        	PCB_relay2_off();
        	PCB_relay3_off();
        }
    }
}
