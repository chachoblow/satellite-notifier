#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "coordinate.h"
#include "led.h"
#include "satellite.h"
#include "speaker.h"
#include "wiFiHandler.h"
#include "satelliteComputer.h"
#include "ledMatrix.h"
#include "satelliteToLedConverter.h"
#include "seedHandler.h"
#include "secrets.h"

#define RXD2 16
#define TXD2 17

// WiFi / Connection
WiFiHandler wiFiHandler(Secrets::SSID, Secrets::PASSWORD);

// Satellites
SatelliteComputer satelliteComputer;
std::vector<Satellite> satellites;

// LED matrix
const int MATRIX_WIDTH = 9;
const int MATRIX_HEIGHT = 9;
LedMatrix ledMatrix(MATRIX_WIDTH, MATRIX_HEIGHT);
std::vector<Coordinate> leds;
const Coordinate CENTER(
    SatelliteConstants::MY_LAT, SatelliteConstants::MY_LNG);
SatelliteToLedConverter satelliteToLedConverter(
    MATRIX_WIDTH, MATRIX_HEIGHT,
    SatelliteConstants::SEARCH_RADIUS, SatelliteConstants::SEARCH_RADIUS,
    CENTER);

// Speaker
// Speaker speaker(0, 8, 19);

// Seed
HardwareSerial SerialSeed(2);
SeedHandler seedHandler(SerialSeed);

void setup()
{
    Serial.begin(9600);
    delay(100);
    wiFiHandler.connectToWiFi();
    ledMatrix.initialize();
    delay(500);
}

bool probeIntervalElapsed()
{
    bool firstConnection = satelliteComputer.lastConnectionTime == 0;
    bool intervalElapsed = millis() - satelliteComputer.lastConnectionTime > N2YORequestConstants::PROBE_INTERVAL;
    return firstConnection || intervalElapsed;
}

void loop()
{
    if (!wiFiHandler.wiFiConnected)
    {
        wiFiHandler.connectToWiFi();
    }
    else if (probeIntervalElapsed())
    { 
        auto satellites = satelliteComputer.fetchSatellites();
        if (satellites.size() > 0)
        {
            // TODO: Turn this into a generic function in LedMatrix which takes any vector of
            //  Coordinates and transforms them into valid matrix coordinates. 
            auto leds = satelliteToLedConverter.convert(satellites);
            ledMatrix.update(leds);
            seedHandler.updateSeed(satellites);
        }
    }
}