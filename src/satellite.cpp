#include "satellite.h"

Satellite::Satellite() = default;

Satellite::Satellite(float altitude, Coordinate coordinates, String name)
{
    this->altitude = altitude;
    this->coordinates = coordinates;
    this->name = name;
}