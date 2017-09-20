/****************************************************************
* FILENAME:     COMM_handlers.c
* DESCRIPTION:  Communication handlers
* AUTHOR:       Mitja Nemec
*
****************************************************************/

#include    "COMM_handlers.h"

bool send_ch1 = TRUE;
bool send_ch2 = TRUE;
bool send_ch3 = FALSE;
bool send_ch4 = FALSE;
bool send_ch5 = FALSE;
bool send_ch6 = FALSE;
bool send_ch7 = FALSE;
bool send_ch8 = FALSE;
int points_to_send = SAMPLE_POINTS;

bool dlog_buffers_sent = TRUE;

// buffer-ja za COBS kodiranje in dekodiranje
// pototipi funkcij
inline float get_float_from_int(int *ptr);
void int_from_float(float number, int *ptr_to_int);

// rx handlerji
void UART_ch_1(int *data);
void UART_ch_2(int *data);
void UART_ch_3(int *data);
void UART_ch_4(int *data);
void UART_ch_5(int *data);
void UART_ch_6(int *data);
void UART_ch_7(int *data);
void UART_ch_8(int *data);
void dlog_sent(void);

void UART_dlog_prescalar(int *data);
void UART_dlog_send_parameters(int *data);

void UART_send_parameters(int *data);
void UART_u_out_amp(int *data);
void UART_u_out_mode(int *data);
void UART_ctrl_type(int *data);

// tx handlerji
inline void send_dlog_ch1(void);
#if DLOG_GEN_NR > 1
inline void send_dlog_ch2(void);
#endif
#if DLOG_GEN_NR > 2
inline void send_dlog_ch3(void);
#endif
#if DLOG_GEN_NR > 3
inline void send_dlog_ch4(void);
#endif
#if DLOG_GEN_NR > 4
inline void send_dlog_ch5(void);
#endif
#if DLOG_GEN_NR > 5
inline void send_dlog_ch6(void);
#endif
#if DLOG_GEN_NR > 6
inline void send_dlog_ch7(void);
#endif
#if DLOG_GEN_NR > 7
inline void send_dlog_ch8(void);
#endif
void send_dlog_params(void);
void send_parameters(void);

extern float u_out_rms_ref ;
extern bool amp_control;
#include    "PER_int.h"
extern enum OUT_STATE out_control;

void COMM_initialization(void)
{
    // inicializiram serijko vodilo
    SCI_init(COMM_BAUDRATE);

    // inicializiram LRTME stack
    LRTME_init();

    // registriram funkcijo
    LRTME_receive_register(0x0911, &UART_ch_1);
    LRTME_receive_register(0x0912, &UART_ch_2);
    LRTME_receive_register(0x0913, &UART_ch_3);
    LRTME_receive_register(0x0914, &UART_ch_4);
    LRTME_receive_register(0x0915, &UART_ch_5);
    LRTME_receive_register(0x0916, &UART_ch_6);
    LRTME_receive_register(0x0917, &UART_ch_7);
    LRTME_receive_register(0x0918, &UART_ch_8);

    LRTME_receive_register(0x0920, &UART_dlog_prescalar);
    LRTME_receive_register(0x092A, &UART_dlog_send_parameters);

    LRTME_receive_register(0x0B10, &UART_u_out_amp);
    LRTME_receive_register(0x0B11, &UART_u_out_mode);
    LRTME_receive_register(0x0B12, &UART_ctrl_type);

    LRTME_receive_register(0x0B1A, &UART_send_parameters);

    // pred zagonom pošljem nastavitve, èe GUI sluèajno že teèe
    send_dlog_params();
    send_parameters();
}

#pragma CODE_SECTION(COMM_runtime, "ramfuncs");
void COMM_runtime(void)
{
    int packets_in_waiting;

    // v kolikor ni preveè podatkov za pošiljanje
    packets_in_waiting = LRTME_tx_queue_poll();

    if (   (dlog.mode == Stop)
        && (dlog_buffers_sent == TRUE)
        && (packets_in_waiting < 2))
    {
        dlog_buffers_sent = FALSE;
        if (send_ch1 == TRUE)
        {
            send_dlog_ch1();
        }
        #if DLOG_GEN_NR > 1
        if (send_ch2 == TRUE)
        {
            send_dlog_ch2();
        }
        #endif
        #if DLOG_GEN_NR > 2
        if (send_ch3 == TRUE)
        {
            send_dlog_ch3();
        }
        #endif
        #if DLOG_GEN_NR > 3
        if (send_ch4 == TRUE)
        {
            send_dlog_ch4();
        }
        #endif
        #if DLOG_GEN_NR > 4
        if (send_ch5 == TRUE)
        {
            send_dlog_ch5();
        }
        #endif
        #if DLOG_GEN_NR > 5
        if (send_ch6 == TRUE)
        {
            send_dlog_ch6();
        }
        #endif
        #if DLOG_GEN_NR > 6
        if (send_ch7 == TRUE)
        {
            send_dlog_ch7();
        }
        #endif
        #if DLOG_GEN_NR > 7
        if (send_ch8 == TRUE)
        {
            send_dlog_ch8();
        }
        #endif
        // da pripravim na posiljanje, ce so vsi kanali izklopljeni in se potem ponovno vklopijo
        if ( (send_ch1 == FALSE) && (send_ch2 == FALSE) && (send_ch3 == FALSE) && (send_ch4 == FALSE) &&
             (send_ch5 == FALSE) && (send_ch6 == FALSE) && (send_ch7 == FALSE) && (send_ch8 == FALSE))
        {
            dlog_sent();
        }
    }

    // klièem handler za komunikacijo
    LRTME_stack();
}

void send_dlog_params(void)
{
    static int status[11];

    // status sestavim skupaj iz
    status[0] = send_ch1;
    status[1] = send_ch2;
    status[2] = send_ch3;
    status[3] = send_ch4;
    status[4] = send_ch5;
    status[5] = send_ch6;
    status[6] = send_ch7;
    status[7] = send_ch8;
    status[8] = points_to_send;
    status[9] = dlog.prescalar;
    if (trigger == Ref_cnt)
    {
        status[10] = 0;
    }
    else
    {
        status[10] = 1;
    }
    LRTME_send(0x090A, status, 2 * sizeof(status), NULL);
}

void send_parameters(void)
{
    static int status[11];

    int_from_float(u_out_rms_ref, &status[0]);
    status[2] = amp_control;
    status[3] = out_control;

    LRTME_send(0x0B0A, status, 2 * sizeof(status), NULL);
}

#pragma CODE_SECTION(UART_ch_1, "ramfuncs");
void UART_ch_1(int *data)
{
    if (*data == TRUE)
    {
        send_ch1 = TRUE;
    }
    else
    {
        send_ch1 = FALSE;
    }
    // še pošljem status
    send_dlog_params();
}

#pragma CODE_SECTION(UART_ch_2, "ramfuncs");
void UART_ch_2(int *data)
{
    if (*data == TRUE)
    {
        send_ch2 = TRUE;
    }
    else
    {
        send_ch2 = FALSE;
    }
    // še pošljem status
    send_dlog_params();
}

#pragma CODE_SECTION(UART_ch_3, "ramfuncs");
void UART_ch_3(int *data)
{
    if (*data == TRUE)
    {
        send_ch3 = TRUE;
    }
    else
    {
        send_ch3 = FALSE;
    }
    // še pošljem status
    send_dlog_params();
}

#pragma CODE_SECTION(UART_ch_4, "ramfuncs");
void UART_ch_4(int *data)
{
    if (*data == TRUE)
    {
        send_ch4 = TRUE;
    }
    else
    {
        send_ch4 = FALSE;
    }
    // še pošljem status
    send_dlog_params();
}

#pragma CODE_SECTION(UART_ch_5, "ramfuncs");
void UART_ch_5(int *data)
{
    if (*data == TRUE)
    {
        send_ch5 = TRUE;
    }
    else
    {
        send_ch5 = FALSE;
    }
    // še pošljem status
    send_dlog_params();
}

#pragma CODE_SECTION(UART_ch_6, "ramfuncs");
void UART_ch_6(int *data)
{
    if (*data == TRUE)
    {
        send_ch6 = TRUE;
    }
    else
    {
        send_ch6 = FALSE;
    }
    // še pošljem status
    send_dlog_params();
}

#pragma CODE_SECTION(UART_ch_7, "ramfuncs");
void UART_ch_7(int *data)
{
    if (*data == TRUE)
    {
        send_ch7 = TRUE;
    }
    else
    {
        send_ch7 = FALSE;
    }
    // še pošljem status
    send_dlog_params();
}

#pragma CODE_SECTION(UART_ch_8, "ramfuncs");
void UART_ch_8(int *data)
{
    if (*data == TRUE)
    {
        send_ch8 = TRUE;
    }
    else
    {
        send_ch8 = FALSE;
    }
    // še pošljem status
    send_dlog_params();
}

inline void send_dlog_ch1()
{
    int *int_ptr;

    int_ptr = (void *) DLOG_b_1;
    if (  (send_ch2 == FALSE)
    	&&(send_ch3 == FALSE)
        &&(send_ch4 == FALSE)
        &&(send_ch5 == FALSE)
        &&(send_ch6 == FALSE)
        &&(send_ch7 == FALSE)
        &&(send_ch8 == FALSE))
    {
        LRTME_send(0x0901, int_ptr, points_to_send*2L*2L, &dlog_sent);
    }
    else
    {
        LRTME_send(0x0901, int_ptr, points_to_send*2L*2L, NULL);
    }
}

#if DLOG_GEN_NR > 1
inline void send_dlog_ch2()
{
    int *int_ptr;

    int_ptr = (void *) DLOG_b_2;
    if (  (send_ch3 == FALSE)
        &&(send_ch4 == FALSE)
        &&(send_ch5 == FALSE)
        &&(send_ch6 == FALSE)
        &&(send_ch7 == FALSE)
        &&(send_ch8 == FALSE))
    {
        LRTME_send(0x0902, int_ptr, points_to_send*2L*2L, &dlog_sent);
    }
    else
    {
        LRTME_send(0x0902, int_ptr, points_to_send*2L*2L, NULL);
    }
}
#endif

#if DLOG_GEN_NR > 2
inline void send_dlog_ch3()
{
    int *int_ptr;

    int_ptr = (void *) DLOG_b_3;
    if (  (send_ch4 == FALSE)
        &&(send_ch5 == FALSE)
        &&(send_ch6 == FALSE)
        &&(send_ch7 == FALSE)
        &&(send_ch8 == FALSE))
    {
        LRTME_send(0x0903, int_ptr, points_to_send*2L*2L, &dlog_sent);
    }
    else
    {
        LRTME_send(0x0903, int_ptr, points_to_send*2L*2L, NULL);
    }
}
#endif

#if DLOG_GEN_NR > 3
inline void send_dlog_ch4()
{
    int *int_ptr;

    int_ptr = (void *) DLOG_b_4;
    if (  (send_ch5 == FALSE)
        &&(send_ch6 == FALSE)
        &&(send_ch7 == FALSE)
        &&(send_ch8 == FALSE))
    {
        LRTME_send(0x0904, int_ptr, points_to_send*2L*2L, &dlog_sent);
    }
    else
    {
        LRTME_send(0x0904, int_ptr, points_to_send*2L*2L, NULL);
    }
}
#endif

#if DLOG_GEN_NR > 4
inline void send_dlog_ch5()
{
    int *int_ptr;

    int_ptr = (void *) DLOG_b_5;
    if (  (send_ch6 == FALSE)
        &&(send_ch7 == FALSE)
        &&(send_ch8 == FALSE))
    {
        LRTME_send(0x0905, int_ptr, points_to_send*2L*2L, &dlog_sent);
    }
    else
    {
        LRTME_send(0x0905, int_ptr, points_to_send*2L*2L, NULL);
    }
}
#endif

#if DLOG_GEN_NR > 5
inline void send_dlog_ch6()
{
    int *int_ptr;

    int_ptr = (void *) DLOG_b_6;
    if (  (send_ch7 == FALSE)
        &&(send_ch8 == FALSE))
    {
        LRTME_send(0x0906, int_ptr, points_to_send*2L*2L, &dlog_sent);
    }
    else
    {
        LRTME_send(0x0906, int_ptr, points_to_send*2L*2L, NULL);
    }
}
#endif

#if DLOG_GEN_NR > 6
inline void send_dlog_ch7()
{
    int *int_ptr;

    int_ptr = (void *) DLOG_b_7;
    if (  (send_ch8 == FALSE))
    {
        LRTME_send(0x0907, int_ptr, points_to_send*2L*2L, &dlog_sent);
    }
    else
    {
        LRTME_send(0x0907, int_ptr, points_to_send*2L*2L, NULL);
    }
}
#endif

#if DLOG_GEN_NR > 7
inline void send_dlog_ch8()
{
    int *int_ptr;

    int_ptr = (void *) DLOG_b_8;
    LRTME_send(0x0908, int_ptr, points_to_send*2L*2L, &dlog_sent);
}
#endif

#pragma CODE_SECTION(dlog_sent, "ramfuncs");
void dlog_sent(void)
{
    dlog.mode = Single;
    dlog_buffers_sent = TRUE;
}

#pragma CODE_SECTION(UART_dlog_prescalar, "ramfuncs");
void UART_dlog_prescalar(int *data)
{
    int podatki = *data;
    dlog.prescalar = podatki;
    // še pošljem status
    send_dlog_params();
}

#pragma CODE_SECTION(UART_dlog_points, "ramfuncs");
void UART_dlog_points(int *data)
{
    int podatki = *data;
    if (podatki <= DLOG_GEN_SIZE)
    {
        points_to_send = podatki;
    }
    else
    {
        points_to_send = DLOG_GEN_SIZE;
    }
    // še pošljem status
    send_dlog_params();
}

// v tej funkciji ne uporabim podatkov, ker juh tudi COM handler ne posreduje
#pragma diag_push
#pragma diag_suppress 880
void UART_dlog_send_parameters(int *data)
{
	// PC je zahteval status
	send_dlog_params();
}
#pragma diag_pop

// v tej funkciji ne uporabim podatkov, ker juh tudi COM handler ne posreduje
#pragma diag_push
#pragma diag_suppress 880
void UART_send_parameters(int *data)
{
    // PC je zahteval status
    send_parameters();
}
#pragma diag_pop


void UART_u_out_amp(int *data)
{
    float   temp;
    temp = get_float_from_int(data);

    // okej sedaj imam v "temp" vrednost amplitude
    if ((temp >= 220.0) && (temp <= 240.0))
    {
        u_out_rms_ref = temp;
    }
}

void UART_u_out_mode(int *data)
{
    int podatki = *data;

    if (podatki == 0)
    {
        amp_control = FALSE;
    }
    if (podatki == 1)
    {
        amp_control = TRUE;
    }
    send_parameters();
}

void UART_ctrl_type(int *data)
{
    int podatki = *data;

    if (podatki == 0)
    {
        out_control = PI_ONLY;
    }
    if (podatki == 1)
    {
        out_control = PI_REP;
    }
    if (podatki == 2)
    {
        out_control = REP;
    }
    if (podatki == 3)
    {
        out_control = RES;
    }
    send_parameters();
}

#pragma diag_push
#pragma diag_suppress 179
float get_float_from_int(int *ptr)
#pragma diag_pop
{
    float   temp;
    int     *int_ptr;

    // iz podatkov potegnem kašna je želena amplituda
    int_ptr = (void *)&temp;
    *int_ptr = *ptr;
    // èe je podatek veèji od 16 bitov, moram zamenjati še byte
    int_ptr = int_ptr + 1;
    ptr = ptr + 1;
    *int_ptr = *ptr;

    return(temp);
}

#pragma diag_push
#pragma diag_suppress 179
void int_from_float(float number, int *ptr_to_int)
#pragma diag_pop
{
    float *pointer_to_number_to_send;
    int *p;

    pointer_to_number_to_send = &number;

    p = (void *)pointer_to_number_to_send;

    *ptr_to_int++ = *p++;
    *ptr_to_int++ = *p++;
}
