#include "wiFiHandler.h"

WiFiHandler::WiFiHandler() = default;

WiFiHandler::WiFiHandler(String ssid, String password)
{
    wiFiConnected = false;
    WiFiMulti _wiFiMulti;
    _ssid = ssid;
    _password = password;
}

void WiFiHandler::connectToWiFi()
{
    _wiFiMulti.addAP(_ssid.c_str(), _password.c_str());

    Serial.println();
    Serial.println("Connecting to WiFi...");

    unsigned long wifiConnectionTime = millis();
    const unsigned long maxConnectionTime = 60000L;

    while (_wiFiMulti.run() != WL_CONNECTED && millis() - wifiConnectionTime < maxConnectionTime) 
    {
        delay(500);
    }

    if (_wiFiMulti.run() == WL_CONNECTED)
    {
        wiFiConnected = true;
        Serial.print("Connected to SSID: ");
        Serial.println(WiFi.SSID());
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.println();
    }
    else 
    {
        Serial.println("Could not connect to WiFi.");
        Serial.println();
    }
}
