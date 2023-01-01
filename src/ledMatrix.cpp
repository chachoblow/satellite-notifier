#include "ledMatrix.h"

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
    auto applicableCoordinates = getApplicableCoordinates(coordinates);
    drawMatrix(applicableCoordinates);
    printCoordinatesToSerial(applicableCoordinates);
}

std::vector<Coordinate> LedMatrix::getApplicableCoordinates(const std::vector<Coordinate> &coordinates) const 
{
    std::vector<Coordinate> applicableCoordinates;
    for (const auto coordinate: coordinates)
    {
        const int x = coordinate.x;
        const int y = coordinate.y;
        if (x >= 0 && x < _width && y >= 0 && y < _height)
        {
            applicableCoordinates.push_back(coordinate);
        }
    }
    return applicableCoordinates;
}

void LedMatrix::drawMatrix(const std::vector<Coordinate> &coordinates) 
{
    int pixelValues[LedMatrixConstants::BOARD_WIDTH][LedMatrixConstants::BOARD_HEIGHT] = {0};

    if (coordinates.size() > 0)
    {
        auto brightnessInterval = 255 / coordinates.size();
        for (const auto coordinate: coordinates)
        {
            const auto x = static_cast<int>(coordinate.x);
            const auto y = static_cast<int>(coordinate.y);
            pixelValues[x][y] = pixelValues[x][y] + brightnessInterval;
        }
    }
    
    for (int x = 0; x < LedMatrixConstants::BOARD_WIDTH; x++)
    {
        for (int y = 0; y < LedMatrixConstants::BOARD_HEIGHT; y++)
        {
            _ledMatrix.drawPixel(x, y, pixelValues[x][y]);
        }
    }
}

void LedMatrix::printCoordinatesToSerial(const std::vector<Coordinate> &coordinates) const
{
    Serial.println();
    Serial.println("--- Active Coordinates ---");
    for (const auto coordinate: coordinates)
    {
        auto x = static_cast<int>(coordinate.x);
        auto y = static_cast<int>(coordinate.y);

        Serial.println("x: " + String(x) + ", y: " + String(y));
    }
    Serial.println();
}

std::vector<Coordinate> LedMatrix::transformCoordinates(
    const float xMin, const float xMax, 
    const float yMin, const float yMax, 
    const std::vector<Coordinate> &coordinates) const
{
    std::vector<Coordinate> result;
    for (const auto coordinate: coordinates)
    {
        auto x = linearInterpolate(coordinate.x, xMin, xMax, 0, _width - 1);
        auto y = linearInterpolate(coordinate.y, yMin, yMax, 0, _height - 1);
        Coordinate current(x, y);
        result.push_back(current);
    }
    return result;
}

int LedMatrix::linearInterpolate(
    const int value, 
    const float minInRange, const float maxInRange, 
    const float minOutRange, const float maxOutRange) const 
{
    auto result = (value - minInRange) / (maxInRange - minInRange);
    result = minOutRange + (maxOutRange - minOutRange) * result;
    return result;
}