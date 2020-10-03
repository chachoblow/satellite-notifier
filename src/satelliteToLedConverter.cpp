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

void SatelliteToLedConverter::convert(std::vector<Coordinate> &coordinates)
{
    for (int i = 0; i < coordinates.size(); i++)
    {
        coordinates[i].x = round((coordinates[i].x - _searchMinX) * _scaleVector[0]);
        coordinates[i].y = round((coordinates[i].y - _searchMinY) * _scaleVector[1]);
    }
}