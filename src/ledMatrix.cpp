#include "ledMatrix.h"

int highestSeenLedsCount = 0;

LedMatrix::LedMatrix() = default;

LedMatrix::LedMatrix(int width, int height)
{
    _width = width;
    _height = height;
    _ledMatrix = Adafruit_IS31FL3731();
}

void LedMatrix::initialize()
{
    Serial.println("Initializing LED matrix...");
    if (!_ledMatrix.begin()) 
    {
        Serial.println("IS31 not found.");
        Serial.println();
        while(1);
    }
    Serial.println("IS31 found.");  
    Serial.println();
}

void LedMatrix::update(const std::vector<Coordinate> &coordinates)
{
    auto leds = ledsFromCoordinates(coordinates);
    redrawMatrix(leds);
    printLedsToSerial(leds);
}

bool LedMatrix::coordinateInMatrixRange(const Coordinate &coordinate)  const
{
    const int x = coordinate.x;
    const int y = coordinate.y;
    return x >= 0 && x < _width && y >= 0 && y < _height;
}

std::vector<Led> LedMatrix::ledsFromCoordinates(const std::vector<Coordinate> &coordinates) const 
{
    std::vector<Coordinate> applicableCoordinates;
    for (const auto coordinate: coordinates)
    {
        if (coordinateInMatrixRange(coordinate))
        {
            applicableCoordinates.push_back(coordinate);
        }
    }

    std::vector<Led> leds;
    for (const auto coordinate: applicableCoordinates) 
    {
        bool coordinateIsUnique = true;
        for (auto &led: leds)
        {
            if (coordinate.equals(led.coordinate))
            {
                led.count++;
                coordinateIsUnique = false;
                break;
            }
        }
        
        if (coordinateIsUnique)
        {
            Led led(coordinate, 1);
            leds.push_back(led);
        }
    }
    return leds;
}

void LedMatrix::clearMatrix() 
{
    for (int x = 0; x < LedMatrixConstants::BOARD_WIDTH; x++)
    {
        for (int y = 0; y < LedMatrixConstants::BOARD_HEIGHT; y++)
        {
            _ledMatrix.drawPixel(x, y, 0);
        }
    }
}

void LedMatrix::redrawMatrix(const std::vector<Led> &leds)
{
    clearMatrix();

    for (const auto led: leds)
    {
        // TODO: Consider having brightness correlate to count on LED (e.g., the number of 
        //  satellites pertaining to that LED coordinate). Brightness doesn't seem to have
        //  an effect on the matrix - at least a perceivable one. 
        _ledMatrix.drawPixel(led.coordinate.x, led.coordinate.y, 255);
    }
}

void LedMatrix::printLedsToSerial(const std::vector<Led> &leds) const
{
    Serial.println();
    Serial.println("--- Active LEDs ---");

    for (const auto led: leds)
    {
        auto count = led.count;
        auto x = led.coordinate.x;
        auto y = led.coordinate.y;

        Serial.println("Count: " + String(count) + ", x: " + String(x) + ", y: " + String(y));
    }

    int ledCount = static_cast<int>(leds.size()); 
    highestSeenLedsCount = max(highestSeenLedsCount, ledCount);

    Serial.println();
    Serial.println("Active led count: " + String(ledCount));
    Serial.println("Highest seen leds count: " + String(highestSeenLedsCount));
    Serial.println();
}