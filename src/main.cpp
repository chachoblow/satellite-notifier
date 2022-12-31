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
#include "seedHandler.h"
#include "secrets.h"

#define RXD2 16
#define TXD2 17

// WiFi / Connection
WiFiHandler wiFiHandler(Secrets::SSID, Secrets::PASSWORD);

// Satellites
SatelliteComputer satelliteComputer;

// LED matrix
const int MATRIX_WIDTH = 9;
const int MATRIX_HEIGHT = 9;
LedMatrix ledMatrix(MATRIX_WIDTH, MATRIX_HEIGHT);

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
        auto satelliteCoordinates = satelliteComputer.fetchSatelliteCoordinates();
        if (satelliteCoordinates.size() > 0)
        {
            auto leds = ledMatrix.transformCoordinates(
                SatelliteConstants::SEARCH_X_MIN, SatelliteConstants::SEARCH_X_MAX, 
                SatelliteConstants::SEARCH_Y_MIN, SatelliteConstants::SEARCH_Y_MAX, 
                satelliteCoordinates);
            ledMatrix.update(leds);
            // TODO: Update seedHanlder
            //seedHandler.updateSeed(satellites);
        }
    }
}