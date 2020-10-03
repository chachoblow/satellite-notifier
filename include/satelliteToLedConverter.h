#pragma once

#include <Arduino.h>
#include "coordinate.h"

class SatelliteToLedConverter
{
    public:
        SatelliteToLedConverter();
        SatelliteToLedConverter(int, int, float, float, Coordinate);
        void convert(std::vector<Coordinate>&);
        
    private:
        std::vector<float> _scaleVector;
        Coordinate _searchCenter;
        float _searchMinX;
        float _searchMinY;
};