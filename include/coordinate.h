#pragma once

#include <Arduino.h>

class Coordinate
{
    public:
        float x;
        float y;
        Coordinate();
        Coordinate(float, float);
        bool equals(const Coordinate&) const;
};