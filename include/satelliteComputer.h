#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <vector>
#include "coordinate.h"
#include "satellite.h"
#include "secrets.h"

#define ARDUINOJSON_DECODE_UNICODE 1
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#define NUM_LEDS 16

namespace N2YORequestConstants
{
    // The n2yo API only allows for 1000 transactions per hour. Thus, this probeInterval
    //  must stay above 3600000 / 1000 = 360.
    const unsigned long PROBE_INTERVAL = 5000L;
}

namespace SatelliteConstants
{
    const float MY_LAT = 47.60621;
    const float MY_LNG = -122.33207;
    const float MY_ALT = 0;
    const int SAT_CATEGORY_ID = 0;
    const float SEARCH_RADIUS = 7;
}

class SatelliteComputer
{
    public:
        unsigned long lastConnectionTime;
        SatelliteComputer();
        int fetchSatellites(std::vector<Satellite>&);
        
    private:
        WiFiClientSecure _client;
        const char* _server;
        const char* _rootCa;
        bool makeHttpRequest();
        bool checkHttpStatus();
        bool skipHttpHeaders();
        int deserializeJson(std::vector<Satellite>&);
        int extractResponseValues(DynamicJsonDocument, std::vector<Satellite>&);
};