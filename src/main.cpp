#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <satellite_computer.h>
#include <led_matrix.h>
#include <seed_handler.h>
#include <speaker.h>
#include <wifi_handler.h>
#include <coordinate.h>
#include <secrets.h>

#define RXD2 16
#define TXD2 17

WiFiHandler wiFiHandler(Secrets::SSID, Secrets::PASSWORD);
SatelliteComputer satelliteComputer;
LedMatrix ledMatrix(9, 9);
Speaker speaker(0, 8, 19);
HardwareSerial SerialSeed(2);        // UART2 command
SeedHandler seedHandler(SerialSeed); // Serial to Daisy Seed command

void setup()
{
    Serial.begin(9600);
    delay(100);
    wiFiHandler.connectToWiFi();
    ledMatrix.initialize();
    speaker.initialize();
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
            auto satelliteCoordinates = satelliteComputer.getSatelliteCoordinates(satellites);
            auto leds = ledMatrix.transformCoordinates(
                SatelliteConstants::SEARCH_X_MIN, SatelliteConstants::SEARCH_X_MAX,
                SatelliteConstants::SEARCH_Y_MIN, SatelliteConstants::SEARCH_Y_MAX,
                satelliteCoordinates);
            ledMatrix.update(leds);
            // TODO: Update seedHanlder
            seedHandler.updateSeed(satellites);
            // speaker.player(satellites);
        }
    }
}