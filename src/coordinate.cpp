#include "coordinate.h"

Coordinate::Coordinate() = default;

Coordinate::Coordinate(int x, int y)
{
    this->x = x;
    this->y = y;
}

boolean Coordinate::equals(Coordinate other)
{
    return this->x == other.x && this->y == other.y;
}