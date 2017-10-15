#include "mbed.h"
#include "SWO.h"
#include <vector>

#define D_SWO    1  //Enable SWO output

#if (D_SWO == 1)
SWO_Channel swo("channel");
#else
static Serial s_debug_serial_port(SERIAL_TX, SERIAL_RX);
#endif

static std::vector<DigitalOut*> s_leds;

static DigitalOut led1(LED1);
static DigitalOut led2(LED2);
static DigitalOut led3(LED6);
static DigitalOut led4(LED5);

static Thread s_thread_blink;
static Thread s_thread_recurrent_1;

static EventQueue s_eq_recurrent_1;

static Timer s_timer_1;

void event_proc_recurrent_1(char c)
{
    #if (D_SWO == 1)
        swo.printf("[event_proc_recurrent_1] Elapsed %d ms\n",s_timer_1.read_ms());
    #else
        printf("[event_proc_recurrent_1] Elapsed %d ms\n",s_timer_1.read_ms());
    #endif
    
}

void thread_proc_blink(std::vector<DigitalOut*> *pleds)
{
    while (true)
    {
        for (std::vector<DigitalOut*>::iterator it = pleds->begin(); it != pleds->end(); it++)
        {
            (*it)->write(!((*it)->read()));

            wait_ms(125);
        }
    }
}

int main()
{
    #if (D_SWO == 1)
    swo.printf("Lablet RTOS Demo #1 main()...\n");
    #else
    s_debug_serial_port.baud(115200);
    printf("Lablet RTOS Demo #1 main()...\n");
    #endif

    s_leds.push_back(&led1);
    s_leds.push_back(&led2);
    s_leds.push_back(&led3);
    s_leds.push_back(&led4);

    s_eq_recurrent_1.call_every(1000,event_proc_recurrent_1,'.');

    s_thread_blink.start(callback(thread_proc_blink, &s_leds));

    s_timer_1.start();
    s_thread_recurrent_1.start(callback(&s_eq_recurrent_1,&EventQueue::dispatch_forever));
}
