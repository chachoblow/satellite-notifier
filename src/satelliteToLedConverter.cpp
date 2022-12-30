#include "satelliteToLedConverter.h"

SatelliteToLedConverter::SatelliteToLedConverter() = default;

SatelliteToLedConverter::SatelliteToLedConverter(int matrixWidth, int matrixHeight, float searchWidth, float searchHeight, Coordinate searchCenter)
{
    float scaleX = matrixWidth / searchWidth;
    float scaleY = matrixHeight / searchHeight;
    _scaleFactors[0] = scaleX;
    _scaleFactors[1] = scaleY;

    _searchCenter = searchCenter;
    _searchMins[0] = searchCenter.x - (searchWidth / 2);
    _searchMins[1] = searchCenter.y - (searchHeight / 2); 
}

std::vector<Coordinate> SatelliteToLedConverter::convert(std::vector<Satellite> &satellites) const
{
    std::vector<Coordinate> leds;
    for (const auto satellite: satellites)
    {
        float x = round((satellite.coordinate.x - _searchMins[0]) * _scaleFactors[0]);
        float y = round((satellite.coordinate.y - _searchMins[1]) * _scaleFactors[1]);

        Coordinate current(x, y);
        leds.push_back(current);
    }
    return leds;
}