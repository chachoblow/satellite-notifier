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

        float coordinateSum = abs(current.latitude) + abs(current.longitude);
        coordinateSum = map(coordinateSum, -270, 270, 0, 255);
        values.push_back((int)coordinateSum);

        // TODO: Altitudes seem to be either really large numbers, or really small. When
        //  they are mapped, this leads to either zero, or numbers close to 255.
        float altitude = current.altitude;
        altitude = constrain(altitude, 0, 100000000);
        altitude = map(altitude, 0, 100000000, 0, 255);
        values.push_back((int)altitude);
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
