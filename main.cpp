#include "mbed.h"

#include <vector>

static std::vector<DigitalOut> s_leds;

static DigitalOut led1(LED1);
static DigitalOut led2(LED2);
static DigitalOut led3(LED6);
static DigitalOut led4(LED5);

static Thread s_thread_blink;

void thread_proc_blink(std::vector<DigitalOut> *pleds)
{
    while (true)
    {
        for (std::vector<DigitalOut>::iterator it = pleds->begin(); it != pleds->end(); it++)
        {
            it->write(!it->read());

            wait_ms(125);
        }
    }
}

int main()
{
    s_leds.push_back(led1);
    s_leds.push_back(led2);
    s_leds.push_back(led3);
    s_leds.push_back(led4);

    s_thread_blink.start(callback(thread_proc_blink, &s_leds));
}