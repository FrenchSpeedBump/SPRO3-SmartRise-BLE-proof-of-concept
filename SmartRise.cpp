#include <stdio.h>
#include <SmartRise.hpp>

SmartRise::SmartRise()
{
    counter = 0;
}
void SmartRise::initialise()
{
    printf("SmartRise initialized\n");
    beacon.startAdvertising();
    //scanner.initialise();
    //scanner.startScan();
}
void SmartRise::update()
{
    counter++;
    printf("Counter: %d\n",counter);
    //scanner.update();
}