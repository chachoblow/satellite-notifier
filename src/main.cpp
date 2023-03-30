#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <satellite_computer.h>
#include <led_matrix.h>
#include <seed_handler.h>
#include <speaker.h>
#include <wifi_handler.h>
#include <coordinate.h>
#include <compass.h>
#include <secrets.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <QMC5883LCompass.h>

#define RXD2 16
#define TXD2 17
#define RXD0 3
#define TXD0 1

WiFiHandler wiFiHandler(Secrets::SSID, Secrets::PASSWORD);
SatelliteComputer satelliteComputer;
Compass compass;
LedMatrix ledMatrix(9, 9);
//  HardwareSerial SerialSeed(2);
//  SeedHandler seedHandler(SerialSeed);

TinyGPSPlus gps;
HardwareSerial SerialGPS(2);

void setup()
{
    Serial.begin(9600);
    delay(100);
    SerialGPS.begin(9600, SERIAL_8N1, RXD2, TXD2);
    wiFiHandler.connectToWiFi();
    ledMatrix.initialize();
    compass.initialize();
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
        // auto satellites = satelliteComputer.fetchSatellites();
        // if (satellites.size() > 0)
        // {
        //     auto satelliteCoordinates = satelliteComputer.getSatelliteCoordinates(satellites);
        //     auto leds = ledMatrix.transformCoordinates(
        //         SatelliteConstants::SEARCH_X_MIN, SatelliteConstants::SEARCH_X_MAX,
        //         SatelliteConstants::SEARCH_Y_MIN, SatelliteConstants::SEARCH_Y_MAX,
        //         satelliteCoordinates);
        //     ledMatrix.update(leds);
        //     // TODO: Update seedHanlder
        //     // seedHandler.updateSeed(satellites);
        // }

        // while (SerialGPS.available() > 0)
        // {
        //     Serial.println("Encoding SerialGPS");
        //     gps.encode(SerialGPS.read());
        // }

        // Serial.print("LAT=");
        // Serial.println(gps.location.lat(), 6);
        // Serial.print("LONG=");
        // Serial.println(gps.location.lng(), 6);
        // Serial.print("ALT=");
        // Serial.println(gps.altitude.meters());
        // Serial.print("SATS=");
        // Serial.println(gps.satellites.value());
        // Serial.println();
    }
    compass.bearing();
    delay(100);
}