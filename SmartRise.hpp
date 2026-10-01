#pragma once

//#include "BleScanner.hpp"
#include "Beacon.hpp"

class SmartRise
{
private: 
    int counter;
    //BleScanner scanner;
    Beacon beacon;

public:
    SmartRise();

    void initialise();
    void update();
};