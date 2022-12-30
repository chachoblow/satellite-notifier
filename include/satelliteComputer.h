#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <vector>
#include "constants.h"
#include "coordinate.h"
#include "satellite.h"

#define ARDUINOJSON_DECODE_UNICODE 1
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#define NUM_LEDS 16

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