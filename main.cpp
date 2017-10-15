#include "mbed.h"

#include <vector>

#define DEBUG_CHANNEL_SERIAL 1
#define DEBUG_CHANNEL_SWO 1
#define DEBUG_CHANNEL_RTT 1

#if (DEBUG_CHANNEL_SWO)
#include "SWO.h"
SWO_Channel swo("channel");
#endif

#if (DEBUG_CHANNEL_SERIAL)
static Serial s_debug_serial_port(SERIAL_TX, SERIAL_RX);
#endif

#if (DEBUG_CHANNEL_RTT)
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_RTT.h"
#endif

static std::vector<DigitalOut *> s_leds;

static DigitalOut led1(LED1);
static DigitalOut led2(LED2);
static DigitalOut led3(LED6);
static DigitalOut led4(LED5);

static Thread s_thread_blink;
static Thread s_thread_recurrent_1;

static EventQueue s_eq_recurrent_1;

static Timer s_timer_1;

int led1_state;
int led2_state;
int led3_state;
int led4_state;

void event_proc_recurrent_1(char c)
{
#if (DEBUG_CHANNEL_SWO)
    swo.printf("[event_proc_recurrent_1 - SWO] Elapsed %d ms\n", s_timer_1.read_ms());
#endif

#if (DEBUG_CHANNEL_SERIAL)
    printf("[event_proc_recurrent_1 - SERIAL] Elapsed %d ms\n", s_timer_1.read_ms());
#endif

#if (DEBUG_CHANNEL_RTT)
    char buf[128];
    sprintf(buf, "[event_proc_recurrent_1 - RTT] Elapsed %d ms\n", s_timer_1.read_ms());
    SEGGER_RTT_WriteString(0, buf);
#endif
}

void thread_proc_blink(std::vector<DigitalOut *> *pleds)
{
    while (true)
    {
        for (std::vector<DigitalOut *>::iterator it = pleds->begin(); it != pleds->end(); it++)
        {
            (*it)->write(!((*it)->read()));

            led1_state=(*pleds)[0]->read();
            led2_state=(*pleds)[1]->read();
            led3_state=(*pleds)[2]->read();
            led4_state=(*pleds)[3]->read();

            wait_ms(125);
        }
    }
}

int main()
{

#if (DEBUG_CHANNEL_SWO)
    swo.printf("[SWO DEBUG CHANNEL] Lablet RTOS Demo #1 main()...\n");
#endif

#if (DEBUG_CHANNEL_SERIAL)
    printf("[SERIAL DEBUG CHANNEL] Lablet RTOS Demo #1 main()...\n");
#endif

#if (DEBUG_CHANNEL_RTT)
    SEGGER_SYSVIEW_Conf();
    SEGGER_RTT_WriteString(0,"[RTT DEBUG CHANNEL] Lablet RTOS Demo #1 main()...\n");
#endif

    s_leds.push_back(&led1);
    s_leds.push_back(&led2);
    s_leds.push_back(&led3);
    s_leds.push_back(&led4);

    s_eq_recurrent_1.call_every(1000, event_proc_recurrent_1, '.');

    s_thread_blink.start(callback(thread_proc_blink, &s_leds));

    s_timer_1.start();
    s_thread_recurrent_1.start(callback(&s_eq_recurrent_1, &EventQueue::dispatch_forever));
}
