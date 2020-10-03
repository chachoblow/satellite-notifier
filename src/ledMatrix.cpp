#include "ledMatrix.h"

int highestLedCount = 0;

LedMatrix::LedMatrix() = default;

LedMatrix::LedMatrix(int width, int height)
{
    _width = width;
    _height = height;
    _ledMatrix = Adafruit_IS31FL3731();
}

void LedMatrix::initialize()
{
    if (!_ledMatrix.begin()) 
    {
        Serial.println("ES31 not found.");
        while(1);
    }
    Serial.println("IS31 found.");  
}

void LedMatrix::update(std::vector<Coordinate> &coordinates)
{
    buildLeds(coordinates);
    updateLeds();
    printLedsToSerial();
}

void LedMatrix::buildLeds(std::vector<Coordinate> &coordinates)
{
    _leds.clear();

    for (int i = 0; i < coordinates.size(); i++)
    {
        Coordinate coordinate = coordinates[i];

        if (coordinate.x < 0 || coordinate.x > _width || coordinate.y < 0 || coordinate.y > _height)
        {
            continue;
        }

        bool isUnique = true;

        for (int j = 0; j < _leds.size(); j++)
        {
            if (coordinate.equals(_leds[j].coordinate))
            {
                _leds[j].count++;
                isUnique = false;
                break;
            }
        }

        if (isUnique)
        {
            Led led(coordinate, 1);
            _leds.push_back(led);
        }
    }
}

void LedMatrix::updateLeds()
{
    int ledCount = static_cast<int>(_leds.size()); 
    highestLedCount = max(highestLedCount, ledCount);

    for (int x = 0; x < LedMatrixConstants::BOARD_WIDTH; x++)
    {
        for (int y = 0; y < LedMatrixConstants::BOARD_HEIGHT; y++)
        {
            Coordinate current(x, y);
            bool active = false;

            for (int i = 0; i < _leds.size(); i++)
            {
                if (_leds[i].coordinate.equals(current))
                {
                    uint16_t brightness = static_cast<uint16_t>(_leds[i].count * 51);
                    _ledMatrix.drawPixel(x, y, brightness);
                    active = true;
                    break;
                }
            }

            if (!active)
            {
                _ledMatrix.drawPixel(x, y, 0);
            }
        }
    }
}

void LedMatrix::printLedsToSerial()
{
    Serial.println();
    Serial.println("--- Active LEDs ---");

    for (int i = 0; i < _leds.size(); i++)
    {
        int count = _leds[i].count;
        int x = _leds[i].coordinate.x;
        int y = _leds[i].coordinate.y;
        Serial.println("Count: " + String(count) + " x: " + String(x) + " y: " + String(y));
    }

    Serial.println();
    Serial.println("Highest led count: " + String(highestLedCount));
    Serial.println();
}