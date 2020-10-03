#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "constants.h"
#include "coordinate.h"
#include "led.h"
#include "wiFiHandler.h"
#include "satelliteComputer.h"
#include "ledMatrix.h"
#include "satelliteToLedConverter.h"

WiFiHandler wiFiHandler("DucksBeakTheBears", "ChampKlein90");
SatelliteComputer satelliteComputer;
const unsigned long probeInterval = 5000L;
std::vector<Coordinate> satellites;
const int MATRIX_WIDTH = 9;
const int MATRIX_HEIGHT = 9;
const Coordinate CENTER(
    SatelliteConstants::MY_LAT, SatelliteConstants::MY_LNG);
LedMatrix ledMatrix(MATRIX_WIDTH, MATRIX_HEIGHT);
SatelliteToLedConverter satelliteToLedConverter(
    MATRIX_WIDTH, MATRIX_HEIGHT, 
    SatelliteConstants::SEARCH_RADIUS, SatelliteConstants::SEARCH_RADIUS, 
    CENTER);
    
void setup() 
{
    Serial.begin(9600);
    delay(100);
    wiFiHandler.connectToWiFi();
    ledMatrix.initialize();
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

    if (wiFiHandler.wiFiConnected && probeIntervalElapsed())
    {
        int satCount = satelliteComputer.fetchSatellites(satellites);
        if (satCount != -1) 
        {
            satelliteToLedConverter.convert(satellites);
            ledMatrix.update(satellites);
        }
    }
    else if (!wiFiHandler.wiFiConnected)
    {
        wiFiHandler.connectToWiFi();
    }
}