#pragma once

#include <Arduino.h>
#include <vector>
#include "satelliteComputer.h"

namespace SpeakerConstants
{
    const uint8_t CHANNEL = 0;
    const uint8_t BITS = 8;
    const uint8_t PIN = 23;
    const int MAX_VALUES = 256;
    const float CONVERSION_FACTOR = (2 * PI) / MAX_VALUES;
}

class Speaker
{
    public: 
        Speaker();
        Speaker(uint8_t, uint8_t, uint8_t);
        void initialize();
        // TODO: Don't have this depend on Satellite. Instead, something more generic.
        void player(std::vector<Satellite>&);

    private:
        uint8_t _channel;
        uint8_t _bits;
        uint8_t _pin;
        uint8_t _step;
        std::vector<uint8_t> sinValues;
        int _counter;
        void SmoothStep(float, float, float, uint32_t);
        float SmoothStepIncrement(float);
        float catmullrom(float, float, float, float, float);
};