#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
#include <cmath>
#include "constants.h"
#include "coordinate.h"
#include "led.h"

class LedMatrix
{
    public:
        LedMatrix();
        LedMatrix(int, int);
        void initialize();
        void update(std::vector<Coordinate>&);
        void updateBrightness();

    private:
        Adafruit_IS31FL3731 _ledMatrix;
        std::vector<Led> _leds;
        int _width;
        int _height;
        void buildLeds(std::vector<Coordinate>&);
        void clearLeds();
        void updateLeds();
        void printLedsToSerial();
};