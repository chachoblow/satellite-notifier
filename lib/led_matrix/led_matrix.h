#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
#include <cmath>
#include <vector>
#include <coordinate.h>

namespace LedMatrixConstants
{
    const int BOARD_WIDTH = 16;
    const int BOARD_HEIGHT = 9;
}

class LedMatrix
{
public:
    LedMatrix();
    LedMatrix(int, int);
    void initialize();
    void update(const std::vector<Coordinate<int>> &);
    std::vector<Coordinate<int>> transformCoordinates(const float, const float, const float, const float, const std::vector<Coordinate<float>> &) const;
    std::vector<Coordinate<int>> getApplicableCoordinates(const std::vector<Coordinate<int>> &) const;

private:
    Adafruit_IS31FL3731 _ledMatrix;
    int _width;
    int _height;
    void drawMatrix(const std::vector<Coordinate<int>> &);
    void printCoordinatesToSerial(const std::vector<Coordinate<int>> &) const;
    int linearInterpolate(const float, const float, const float, const float, const float) const;
};