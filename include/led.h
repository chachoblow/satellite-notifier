#pragma once

#include <arduino.h>
#include "coordinate.h"

class Led
{
    public:
        Coordinate coordinate;
        int count;
        Led();
        Led(Coordinate, int);
};