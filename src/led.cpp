#include "led.h"

Led::Led() = default;

Led::Led(Coordinate coordinate, int count)
{
    this->coordinate = coordinate;
    this->count = count;
};