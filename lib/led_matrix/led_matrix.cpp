#include <led_matrix.h>

LedMatrix::LedMatrix() = default;

LedMatrix::LedMatrix(int width, int height)
{
    _width = width;
    _height = height;
}

void LedMatrix::initialize()
{
    _ledMatrix = Adafruit_IS31FL3731();
    Serial.println("Initializing LED matrix...");
    if (!_ledMatrix.begin())
    {
        Serial.println("IS31 not found.");
        Serial.println();
        while (1)
            ;
    }
    Serial.println("IS31 found.");
    Serial.println();
}

void LedMatrix::update(const std::vector<Coordinate<int>> &coordinates, const float brightness)
{
    auto applicableCoordinates = getApplicableCoordinates(coordinates);

    if (applicableCoordinates.size() > 0)
    {
        // Purposefully clearing the entire matrix. If the _width and _height are less than the
        //  matrix's full width and height, LEDs can get "stuck" on.
        int pixels[LedMatrixConstants::BOARD_WIDTH][LedMatrixConstants::BOARD_HEIGHT] = {{0}};

        auto brightnessInterval = 255 / applicableCoordinates.size();
        for (const auto coordinate : applicableCoordinates)
        {
            const auto x = static_cast<int>(coordinate.x);
            const auto y = static_cast<int>(coordinate.y);
            pixels[x][y] = pixels[x][y] + brightnessInterval;
        }

        for (int x = 0; x < LedMatrixConstants::BOARD_WIDTH; x++)
        {
            for (int y = 0; y < LedMatrixConstants::BOARD_HEIGHT; y++)
            {
                _ledMatrix.drawPixel(x, y, pixels[x][y] * brightness);
            }
        }
    }
}

std::vector<Coordinate<int>> LedMatrix::getApplicableCoordinates(const std::vector<Coordinate<int>> &coordinates) const
{
    std::vector<Coordinate<int>> applicableCoordinates;
    for (const auto coordinate : coordinates)
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

std::vector<Coordinate<int>> LedMatrix::transformCoordinates(
    const float xMin, const float xMax,
    const float yMin, const float yMax,
    const std::vector<Coordinate<float>> &coordinates) const
{
    std::vector<Coordinate<int>> result;
    for (const auto coordinate : coordinates)
    {
        auto x = linearInterpolate(coordinate.x, xMin, xMax, 0, _width - 1);
        auto y = linearInterpolate(coordinate.y, yMin, yMax, 0, _height - 1);
        Coordinate<int> current{x, y};
        result.push_back(current);
    }
    return result;
}

int LedMatrix::linearInterpolate(
    const float value,
    const float minInRange, const float maxInRange,
    const float minOutRange, const float maxOutRange) const
{
    auto result = (value - minInRange) / (maxInRange - minInRange);
    result = minOutRange + (maxOutRange - minOutRange) * result;
    return round(result);
}

void LedMatrix::printCoordinatesToSerial(const std::vector<Coordinate<int>> &coordinates) const
{
    Serial.println();
    Serial.println("--- Matrix Coordinates ---");
    for (const auto coordinate : coordinates)
    {
        auto x = static_cast<int>(coordinate.x);
        auto y = static_cast<int>(coordinate.y);

        Serial.println("x: " + String(x) + ", y: " + String(y));
    }
    Serial.println();
}