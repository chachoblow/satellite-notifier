#pragma once

#include <Arduino.h>
#include "constants.h"
#include "satellite.h"

class Speaker
{
    public: 
        Speaker();
        Speaker(uint8_t, uint8_t, uint8_t);
        void initialize();
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