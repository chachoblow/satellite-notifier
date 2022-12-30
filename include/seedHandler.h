#pragma once

#include <Arduino.h>
#include <vector>
#include "satellite.h"

#define RXD2 16
#define TXD2 17

class SeedHandler
{
    public:
        SeedHandler(HardwareSerial &serialSeed);
        void updateSeed(std::vector<Satellite>&);

    private:
        HardwareSerial &_serialSeed;
};