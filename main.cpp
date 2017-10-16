#include "mbed.h"

#include <vector>

#define DEBUG_CHANNEL_SERIAL 0
#define DEBUG_CHANNEL_SWO 0
#define DEBUG_CHANNEL_RTT 1

#if (DEBUG_CHANNEL_SWO)
#include "SWO.h"
SWO_Channel swo("channel");
#endif

#if (DEBUG_CHANNEL_SERIAL)
static Serial s_debug_serial_port(SERIAL_TX, SERIAL_RX);
#endif

#if (DEBUG_CHANNEL_RTT)
#include "SEGGER_RTT.h"
#define RTT_TID_LOG 0
#define RTT_TID_INFO 1
static Thread s_thread_rtt_input;
static EventQueue s_eq_rtt_input;
#endif

static std::vector<DigitalOut> s_leds;

static DigitalOut led1(LED1);
static DigitalOut led2(LED2);
static DigitalOut led3(LED6);
static DigitalOut led4(LED5);

static DigitalIn btn(BUTTON1);

static Thread s_thread_blink;
static Thread s_thread_recurrent_1;
static Thread s_thread_button_poll;

static EventQueue s_eq_recurrent_1;
static EventQueue s_eq_button_poll;

static Timer s_timer_1;

int led1_state;
int led2_state;
int led3_state;
int led4_state;

int latest_button_state;

void event_proc_button_poll()
{
    int state = btn.read();

    if (state != latest_button_state)
    {
        #if (DEBUG_CHANNEL_RTT)
        SEGGER_RTT_SetTerminal(RTT_TID_INFO);
        SEGGER_RTT_printf(0, "%s[RTT DEBUG CHANNEL] Stato pulsante : %s %s\n",
                          RTT_CTRL_TEXT_BRIGHT_MAGENTA,
                          state ? "ON" : "OFF",
                          RTT_CTRL_RESET);
        #endif

        latest_button_state = state;
    }
}

void event_proc_recurrent_1(char c)
{
#if (DEBUG_CHANNEL_SWO)
    swo.printf("[event_proc_recurrent_1 - SWO] Elapsed %d ms (%c)\n", s_timer_1.read_ms(), c);
#endif

#if (DEBUG_CHANNEL_SERIAL)
    printf("[event_proc_recurrent_1 - SERIAL] Elapsed %d ms (%c)\n", s_timer_1.read_ms(), c);
#endif

#if (DEBUG_CHANNEL_RTT)
    SEGGER_RTT_SetTerminal(RTT_TID_LOG);
    SEGGER_RTT_printf(0, "[event_proc_recurrent_1 - RTT] Elapsed %d ms (%c)\n", s_timer_1.read_ms(), c);
#endif
}

void thread_proc_blink(std::vector<DigitalOut>* pleds)
{
    while (true)
    {
        for (std::vector<DigitalOut>::iterator it = pleds->begin(); it != pleds->end(); it++)
        {
            it->write(!(it->read()));

            led1_state = ((*pleds)[0]).read();
            led2_state = ((*pleds)[1]).read();
            led3_state = ((*pleds)[2]).read();
            led4_state = ((*pleds)[3]).read();

            wait_ms(125);
        }
    }
}

#if (DEBUG_CHANNEL_RTT)
void event_proc_rtt_input()
{
    if (SEGGER_RTT_HasKey())
    {
        int c = SEGGER_RTT_GetKey();

        switch (c)
        {
            case 'r':
            case 'R':
                NVIC_SystemReset();
                break;
        }
    }
}
#endif

int main()
{

#if (DEBUG_CHANNEL_SWO)
    swo.printf("[SWO DEBUG CHANNEL] Lablet RTOS Demo #1 main()...\n");
#endif

#if (DEBUG_CHANNEL_SERIAL)
    printf("[SERIAL DEBUG CHANNEL] Lablet RTOS Demo #1 main()...\n");
#endif

#if (DEBUG_CHANNEL_RTT)
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);

    SEGGER_RTT_SetTerminal(RTT_TID_INFO);
    SEGGER_RTT_printf(0, "%s[RTT DEBUG CHANNEL] Lablet RTOS Demo #1 main()...%s\n", RTT_CTRL_TEXT_BRIGHT_GREEN, RTT_CTRL_RESET);

    SEGGER_RTT_SetTerminal(RTT_TID_LOG);
#endif

    s_leds.push_back(led1);
    s_leds.push_back(led2);
    s_leds.push_back(led3);
    s_leds.push_back(led4);

    s_eq_recurrent_1.call_every(1000, event_proc_recurrent_1, '.');
    s_eq_button_poll.call_every(10, event_proc_button_poll);

    s_thread_blink.start(callback(thread_proc_blink, &s_leds));

    s_timer_1.start();
    s_thread_recurrent_1.start(callback(&s_eq_recurrent_1, &EventQueue::dispatch_forever));

    latest_button_state = btn.read();
    s_thread_button_poll.start(callback(&s_eq_button_poll, &EventQueue::dispatch_forever));

#if (DEBUG_CHANNEL_RTT)
    s_eq_rtt_input.call_every(1000, event_proc_rtt_input);
    s_thread_rtt_input.start(callback(&s_eq_rtt_input, &EventQueue::dispatch_forever));
#endif
}
