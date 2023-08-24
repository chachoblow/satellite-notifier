#include <seed_handler.h>

SeedHandler::SeedHandler(HardwareSerial &serialSeed) : _serialSeed(serialSeed)
{
    _serialSeed.begin(115200, SERIAL_8N1, RXD2, TXD2); // UART2
}

void SeedHandler::updateSeed(std::vector<Satellite> &satellites)
{
    std::vector<int> values;

    for (int i = 0; i < satellites.size(); i++)
    {
        Satellite current = satellites[i];
        SeedHandler::addNormalizedSatelliteValues(values, current);
    }

    if (_serialSeed.available())
    {
        Serial.println("Sending satellite values:");

        for (int i = 0; i < values.size(); i++)
        {
            Serial.println(values[i]);
            _serialSeed.write(values[i]); // Write to Daisy Seed
        }

        Serial.println("All values sent.");
        _serialSeed.write('\n'); // Write to Daisy Seed
    }
    else
    {
        Serial.println("_serialSeed not available. Satellite values not sent.");
    }
}

void SeedHandler::addNormalizedSatelliteValues(std::vector<int> &values, Satellite satellite)
{
    int normalizedLongitude = map(satellite.longitude, SatelliteConstants::SEARCH_Y_MIN, SatelliteConstants::SEARCH_Y_MAX, 0, 255);
    int normalizedLatitude = map(satellite.latitude, SatelliteConstants::SEARCH_X_MIN, SatelliteConstants::SEARCH_X_MAX, 0, 255);
    int normalizedAltitude = static_cast<int>(satellite.altitude) % 100;
    int normalizedId = satellite.id % 10;

    values.push_back(normalizedLongitude);
    values.push_back(normalizedLatitude);
    values.push_back(normalizedAltitude);
    values.push_back(normalizedId);
}
