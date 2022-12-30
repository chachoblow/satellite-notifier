#include "satelliteComputer.h"

SatelliteComputer::SatelliteComputer()
{
    WiFiClientSecure _client;
    lastConnectionTime = 0;
    _server = "api.n2yo.com";
    _rootCa = \
        "-----BEGIN CERTIFICATE-----\n" \
        "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
        "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
        "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
        "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
        "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
        "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
        "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
        "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
        "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
        "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
        "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
        "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
        "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
        "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
        "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
        "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
        "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
        "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
        "-----END CERTIFICATE-----\n";
}

int SatelliteComputer::fetchSatellites(std::vector<Satellite> &satellites) 
{
    _client.stop();
    _client.setCACert(_rootCa);

    Serial.println("Connecting to satellite service.");
    
    if (_client.connect(_server, 443)) 
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
        Serial.println();
        Serial.println("Connection failed.");
        char buf[200];
        int err = _client.lastError(buf, 199);
        buf[199] = '\0';
        Serial.println("Last SSL error was:");
        Serial.println(buf);
        Serial.print("ERRCODE: "); Serial.println(err);
        Serial.println();
        return -1;
    }
    return 0;
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
    _client.print(F("Host: "));
    _client.println(_server);
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

int SatelliteComputer::deserializeJson(std::vector<Satellite> &satellites)
{
    StaticJsonDocument<256> filter;
    filter["info"]["satcount"] = true;
    filter["above"][0]["satname"] = true;
    filter["above"][0]["satlat"] = true;
    filter["above"][0]["satlng"] = true;
    filter["above"][0]["satalt"] = true;

    // Enough space for up to 40 satellites (could probably trim down if needed).
    const size_t capacity = JSON_ARRAY_SIZE(40) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 40 * JSON_OBJECT_SIZE(4);
    DynamicJsonDocument doc(capacity);
    DeserializationError error = ArduinoJson::deserializeJson(doc, _client, DeserializationOption::Filter(filter));
    if (error) 
    {
        Serial.print(F("deserializeJson() returned: "));
        Serial.println(error.c_str());
        return -1;
    }
    return extractResponseValues(doc, satellites);
}

int SatelliteComputer::extractResponseValues(DynamicJsonDocument doc, std::vector<Satellite> &satellites)
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
        float satAlt = doc["above"][i]["satalt"];
        
        Serial.println("Name: " + satName + ", Lat: " + String(satLat) + ", Lng: " + String(satLng) + ", Alt: " + String(satAlt));

        Coordinate satCoord(satLat, satLng);
        Satellite current(satAlt, satCoord, satName);
        satellites.push_back(current);
    }

    return satCount;
}











