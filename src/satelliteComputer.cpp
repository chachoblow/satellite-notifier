#include "satelliteComputer.h"

SatelliteComputer::SatelliteComputer()
{
    WiFiClientSecure _client;
    lastConnectionTime = 0;
}

int SatelliteComputer::fetchSatellites(std::vector<Coordinate> &satellites) 
{
    _client.stop();
    
    if (_client.connect("www.n2yo.com", 443)) 
    {
        lastConnectionTime = millis();

        Serial.println("Connected to server.");
        Serial.println();

        if (!(makeHttpRequest() && checkHttpStatus() && skipHttpHeaders())) 
        {
            return -1;
        }

        deserializeJson(satellites);
    }
    else
    {
        Serial.println("Connection failed.");
        Serial.println();
        return -1;
    }
}

bool SatelliteComputer::makeHttpRequest()
{
    String request = 
        "GET /rest/v1/satellite/above/" + 
        String(SatelliteConstants::MY_LAT) + "/" +
        String(SatelliteConstants::MY_LNG) + "/" +
        String(SatelliteConstants::MY_ALT) + "/" + 
        String(SatelliteConstants::SEARCH_RADIUS) + "/" +
        String(SatelliteConstants::SAT_CATEGORY_ID) + "/" +
        "/&apiKey=" + N2YORequestConstants::API_KEY + 
        " HTTP/1.0";
    _client.println(request);
    _client.println(F("Host: www.n2yo.com"));
    _client.println(F("Connection: close"));
    if (_client.println() == 0)
    {
        Serial.println(F("Failed to send reqeust"));
        return false;
    }
    return true;
}

bool SatelliteComputer::checkHttpStatus()
{
    char status[32] = { 0 };
    _client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0)
    {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        return false;
    }
    return true;
}

bool SatelliteComputer::skipHttpHeaders()
{
    char endOfHeaders[] = "\r\n\r\n";
    if (!_client.find(endOfHeaders))
    {
        Serial.println(F("Invalid response"));
        return false;
    }
    return true;
}

int SatelliteComputer::deserializeJson(std::vector<Coordinate> &satellites)
{
    StaticJsonDocument<256> filter;
    filter["info"]["satcount"] = true;
    filter["above"][0]["satname"] = true;
    filter["above"][0]["satlat"] = true;
    filter["above"][0]["satlng"] = true;

    // Enough space for up to 40 satellites (could probably trim down if needed).
    const size_t capacity = JSON_ARRAY_SIZE(40) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 40 * JSON_OBJECT_SIZE(3);
    DynamicJsonDocument doc(capacity);
    DeserializationError error = ArduinoJson6161_11::deserializeJson(doc, _client, DeserializationOption::Filter(filter));
    if (error) 
    {
        Serial.print(F("deserializeJson() returned: "));
        Serial.println(error.c_str());
        return -1;
    }
    return extractResponseValues(doc, satellites);
}

int SatelliteComputer::extractResponseValues(DynamicJsonDocument doc, std::vector<Coordinate> &satellites)
{
    int satCount = doc["info"]["satcount"];

    Serial.println(F("--- Response ---"));
    Serial.print("Satellite count: ");
    Serial.println(satCount);

    satellites.clear();

    for (int i = 0; i < satCount;  i++) 
    {
        String satName = doc["above"][i]["satname"];
        float satLat = doc["above"][i]["satlat"];
        float satLng = doc["above"][i]["satlng"];
        
        Serial.println("Name: " + satName + ", Lat: " + String(satLat) + ", Lng: " + String(satLng));

        Coordinate current(satLat, satLng);
        satellites.push_back(current);
    }

    return satCount;
}










