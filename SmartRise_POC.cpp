#include <stdio.h>
#include "pico/stdlib.h"
#include "SmartRise.hpp"


int main()
{
    stdio_init_all();

    SmartRise smartrise;
    smartrise.initialise();

    while(true)
    {
        smartrise.update();
        sleep_ms(1000);
    }
}
