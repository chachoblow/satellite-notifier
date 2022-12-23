#pragma once

#include <Arduino.h>
#include "coordinate.h"

class Satellite
{
    public:
        float altitude;
        Coordinate coordinates;
        String name;
        Satellite();
        Satellite(float, Coordinate, String);
};