#pragma once

#include <arduino.h>
#include "coordinate.h"

class Led
{
    private:

    public:
        Coordinate coordinate;
        int count;
        Led();
        Led(Coordinate, int);
};