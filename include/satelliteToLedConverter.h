#pragma once

#include <Arduino.h>
#include <vector>
#include "coordinate.h"
#include "satellite.h"

class SatelliteToLedConverter
{
    public:
        SatelliteToLedConverter();
        SatelliteToLedConverter(int, int, float, float, Coordinate);
        std::vector<Coordinate> convert(std::vector<Satellite>&) const;
        
    private:
        float _scaleFactors[2];
        float _searchMins[2];
        Coordinate _searchCenter;
};