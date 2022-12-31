#pragma once

#include <Arduino.h>
#include "coordinate.h"

struct Satellite
{
    int id;
    std::string name;
    Coordinate coordinate;
    float altitude;
};