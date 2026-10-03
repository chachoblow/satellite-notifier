#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <satellite_computer.h>
#include <led_matrix.h>
#include <wifi_handler.h>
#include <coordinate.h>
#include <secrets.h>

const int POT_PIN = 33;

WiFiHandler wiFiHandler(Secrets::SSID, Secrets::PASSWORD);
SatelliteComputer satelliteComputer;
LedMatrix ledMatrix(9, 9);
std::vector<Coordinate<int>> leds;

bool probeIntervalElapsed()
{
    bool firstConnection = satelliteComputer.lastConnectionTime == 0;
    bool intervalElapsed = millis() - satelliteComputer.lastConnectionTime > N2YORequestConstants::PROBE_INTERVAL;
    return firstConnection || intervalElapsed;
}

void updateLedCoordinates(void *pvParameters)
{
    while (true)
    {
        Serial.print("updateLedCoordinates running on core ");
        Serial.println(xPortGetCoreID());
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
                leds = ledMatrix.transformCoordinates(
                    SatelliteConstants::SEARCH_X_MIN, SatelliteConstants::SEARCH_X_MAX,
                    SatelliteConstants::SEARCH_Y_MIN, SatelliteConstants::SEARCH_Y_MAX,
                    satelliteCoordinates);
            }
        }
        // TODO: Adding this longer delay really helped.
        vTaskDelay(100);
        // vTaskDelay(pdMS_TO_TICKS(N2YORequestConstants::PROBE_INTERVAL));
    }
}

void updateMatrix(void *pvParameters)
{
    while (true)
    {
        Serial.print("updateMatrix running on core ");
        Serial.println(xPortGetCoreID());
        auto potValue = analogRead(POT_PIN);
        auto brightness = static_cast<float>(potValue) / 4095;
        ledMatrix.update(leds, brightness);
        vTaskDelay(0);
    }
}

void setup()
{
    Serial.begin(9600);
    delay(100);
    wiFiHandler.connectToWiFi();
    ledMatrix.initialize();

    xTaskCreatePinnedToCore(
        updateLedCoordinates,
        "updateLedCoordinatesTask",
        10000,
        NULL,
        1,
        NULL,
        0);

    xTaskCreatePinnedToCore(
        updateMatrix,
        "updateMatrixTask",
        10000,
        NULL,
        1,
        NULL,
        1);

    delay(500);
}

void loop()
{
    vTaskDelay(0);
}