#pragma once

#include <Arduino.h>
#include <vector>
#include <satellite_computer.h>

#define RXD2 16
#define TXD2 17

class SeedHandler
{
    public:
        SeedHandler(HardwareSerial &serialSeed);
        // TODO: Don't have this depend on Satellite. Instead, something more generic. 
        void updateSeed(std::vector<Satellite>&);

    private:
        HardwareSerial &_serialSeed;
};