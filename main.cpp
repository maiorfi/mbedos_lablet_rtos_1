#include "mbed.h"

#include <vector>

static std::vector<DigitalOut> s_leds;

static DigitalOut led1(LED1);
static DigitalOut led2(LED2);
static DigitalOut led3(LED6);
static DigitalOut led4(LED5);

int main()
{
    s_leds.push_back(led1);
    s_leds.push_back(led2);
    s_leds.push_back(led3);
    s_leds.push_back(led4);

    while (true)
    {
        for (std::vector<DigitalOut>::iterator it=s_leds.begin();it!=s_leds.end();it++)
        {
            it->write(!it->read());

            wait_ms(125);
        }
    }
}