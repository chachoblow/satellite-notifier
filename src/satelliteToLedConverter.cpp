#include "satelliteToLedConverter.h"

SatelliteToLedConverter::SatelliteToLedConverter() = default;

SatelliteToLedConverter::SatelliteToLedConverter(int matrixWidth, int matrixHeight, float searchWidth, float searchHeight, Coordinate searchCenter)
{
    float scaleX = matrixWidth / searchWidth;
    float scaleY = matrixHeight / searchHeight;
    _scaleVector.push_back(scaleX);
    _scaleVector.push_back(scaleY);

    _searchCenter = searchCenter;
    _searchMinX = searchCenter.x - (searchWidth / 2);
    _searchMinY = searchCenter.y - (searchHeight / 2);
}

void SatelliteToLedConverter::convert(std::vector<Satellite> &satellites, std::vector<Coordinate> &leds)
{
    leds.clear();

    for (int i = 0; i < satellites.size(); i++)
    {
        float x = round((satellites[i].coordinates.x - _searchMinX) * _scaleVector[0]);
        float y = round((satellites[i].coordinates.y - _searchMinY) * _scaleVector[1]);

        Coordinate current(x, y);
        leds.push_back(current);
    }
}