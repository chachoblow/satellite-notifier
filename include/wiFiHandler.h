#pragma once

#include <Arduino.h>
#include <WiFiMulti.h>

class WiFiHandler
{
    public:
        bool wiFiConnected;
        WiFiHandler();
        WiFiHandler(String, String);
        void connectToWiFi();

    private:
        String _ssid;
        String _password;
        WiFiMulti _wiFiMulti;
};