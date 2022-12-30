#include "coordinate.h"

Coordinate::Coordinate() = default;

Coordinate::Coordinate(float x, float y)
{
    this->x = x;
    this->y = y;
}

boolean Coordinate::equals(const Coordinate &other) const
{
    return x == other.x && y == other.y;
}