#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
#include <cmath>
#include <vector>
#include "coordinate.h"
#include "led.h"

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
        void update(const std::vector<Coordinate>&);

    private:
        Adafruit_IS31FL3731 _ledMatrix;
        int _width;
        int _height;
        std::vector<Coordinate> getApplicableCoordinates(const std::vector<Coordinate>&) const;
        void drawMatrix(const std::vector<Coordinate>&);
        void printCoordinatesToSerial(const std::vector<Coordinate>&) const;
};