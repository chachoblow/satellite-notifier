#pragma once

#include <Arduino.h>

class Coordinate
{
    private:

    public:
        int x;
        int y;
        Coordinate();
        Coordinate(int, int);
        boolean equals(Coordinate);
};