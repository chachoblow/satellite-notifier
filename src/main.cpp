#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "constants.h"
#include "coordinate.h"
#include "led.h"
#include "satellite.h"
#include "speaker.h"
#include "wiFiHandler.h"
#include "satelliteComputer.h"
#include "ledMatrix.h"
#include "satelliteToLedConverter.h"

// WiFi / Connection
WiFiHandler wiFiHandler("DucksBeakTheBears", "ChampKlein90");
// The n2yo API only allows for 1000 transactions per hour. Thus, this probeInterval
//  must stay above 3600000 / 1000 = 360.
const unsigned long probeInterval = 5000L;

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
Speaker speaker(0, 8, 19);
    
void setup() 
{
    Serial.begin(9600);
    delay(100);
    wiFiHandler.connectToWiFi();
    ledMatrix.initialize();
    speaker.initialize();
}

bool probeIntervalElapsed()
{
    bool firstConnection = satelliteComputer.lastConnectionTime == 0;
    bool intervalElapsed = millis() - satelliteComputer.lastConnectionTime > probeInterval;
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
        int satCount = satelliteComputer.fetchSatellites(satellites);
        if (satCount != -1) 
        {
            satelliteToLedConverter.convert(satellites, leds);
            ledMatrix.update(leds);
        }
    }
    else 
    {
        speaker.player(satellites);
    }
}