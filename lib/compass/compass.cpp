#include <compass.h>

Compass::Compass() = default;

void Compass::initialize()
{
    Serial.println("Initializing compass...");
    QMC5883LCompass _compass;
    _compass.init();
    _compass.setCalibration(-81, 3625, -2518, 2948, -4395, 1073505532);
    Serial.println("Compass initialized.");
    Serial.println();
}

int Compass::bearing()
{
    _compass.read();

    int azimuth = _compass.getAzimuth();
    // 0 - 16: 0 = N, 4 = E, 8 = S, 12 = W.
    int bearing = _compass.getBearing(azimuth);

    Serial.print("Direction: ");
    char myArray[3];
    _compass.getDirection(myArray, azimuth);
    Serial.print(myArray[0]);
    Serial.print(myArray[1]);
    Serial.print(myArray[2]);
    Serial.print(" Bearing reading: ");
    Serial.println(bearing);

    return bearing;
}