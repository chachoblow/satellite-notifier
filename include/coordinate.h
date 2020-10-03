#pragma once

#include <Arduino.h>

class Coordinate
{
    public:
        float x;
        float y;
        Coordinate();
        Coordinate(float, float);
        boolean equals(Coordinate);
};