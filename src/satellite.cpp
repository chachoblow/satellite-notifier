#include "satellite.h"

Satellite::Satellite() = default;

Satellite::Satellite(float altitude, Coordinate coordinate, String name)
{
    this->altitude = altitude;
    this->coordinate = coordinate;
    this->name = name;
}