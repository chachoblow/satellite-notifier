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

std::vector<Coordinate> SatelliteComputer::fetchSatelliteCoordinates()
{
    const auto satellites = fetchSatellites();
    std::vector<Coordinate> result;
    for (const auto satellite: satellites)
    {
        result.push_back(satellite.coordinate);
    }
    return result;
}

std::vector<Satellite> SatelliteComputer::fetchSatellites() 
{
    _client.stop();
    _client.setCACert(_rootCa);

    Serial.println("Connecting to satellite service.");
    
    if (_client.connect(_server, 443)) 
    {
        lastConnectionTime = millis();

        Serial.println("Connected to server.");
        Serial.println();

        try
        {
            makeHttpRequest();
            checkHttpStatus();
            skipHttpHeaders();
            const auto response = deserializeJson();
            const auto satellites = extractResponseValues(response);
            printSatellitesToSerial(satellites);
            return satellites;
        }
        catch(std::runtime_error &e)
        {
            Serial.println(e.what());
            return {};
        }
    }

    Serial.println();
    Serial.println("Connection failed.");
    char buf[200];
    int err = _client.lastError(buf, 199);
    buf[199] = '\0';
    Serial.println("Last SSL error was:");
    Serial.println(buf);
    Serial.print("ERRCODE: "); Serial.println(err);
    Serial.println();

    return {};
}

void SatelliteComputer::makeHttpRequest()
{
    String request = 
        "GET /rest/v1/satellite/above/" + 
        String(SatelliteConstants::MY_LAT) + "/" +
        String(SatelliteConstants::MY_LNG) + "/" +
        String(SatelliteConstants::MY_ALT) + "/" + 
        String(SatelliteConstants::SEARCH_RADIUS) + "/" +
        String(SatelliteConstants::SAT_CATEGORY_ID) + "/" +
        "/&apiKey=" + String(Secrets::API_KEY) + 
        " HTTP/1.0";
    _client.println(request);
    _client.print(F("Host: "));
    _client.println(_server);
    _client.println(F("Connection: close"));
    if (_client.println() == 0)
    {
        throw std::runtime_error("Failed to send request.");
    }
}

void SatelliteComputer::checkHttpStatus()
{
    char status[32] = { 0 };
    _client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0)
    {
        throw std::runtime_error("Unexpected response: " + std::string(status));
    }
}

void SatelliteComputer::skipHttpHeaders()
{
    char endOfHeaders[] = "\r\n\r\n";
    if (!_client.find(endOfHeaders))
    {
        throw std::runtime_error("Invalid response");
    }
}

DynamicJsonDocument SatelliteComputer::deserializeJson()
{
    StaticJsonDocument<256> filter;
    filter["info"]["satcount"] = true;
    filter["above"][0]["satid"] = true;
    filter["above"][0]["satname"] = true;
    filter["above"][0]["satlat"] = true;
    filter["above"][0]["satlng"] = true;
    filter["above"][0]["satalt"] = true;

    // Enough space for up to 40 satellites (could probably trim down if needed).
    const size_t capacity = JSON_ARRAY_SIZE(40) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 40 * JSON_OBJECT_SIZE(5);
    DynamicJsonDocument doc(capacity);
    auto error = ArduinoJson::deserializeJson(doc, _client, DeserializationOption::Filter(filter));
    if (error) 
    {
        throw std::runtime_error("Deserialization error: " + std::string(error.c_str()));
    }
    return doc;
}

std::vector<Satellite> SatelliteComputer::extractResponseValues(const DynamicJsonDocument doc)
{
    std::vector<Satellite> satellites;
    int satCount = doc["info"]["satcount"];
    for (int i = 0; i < satCount;  i++) 
    {
        std::string satName = doc["above"][i]["satname"];
        int satId = doc["above"][i]["satid"];
        float satLat = doc["above"][i]["satlat"];
        float satLng = doc["above"][i]["satlng"];
        float satAlt = doc["above"][i]["satalt"];
        
        Coordinate satCoord(satLat, satLng);
        Satellite current{satId, satName, satCoord, satAlt};
        satellites.push_back(current);
    }
    return satellites;
}

void SatelliteComputer::printSatellitesToSerial(const std::vector<Satellite> &satellites) const
{
    Serial.println("--- Response ---");
    Serial.println("Satellite count: " + String(satellites.size()));
    for (const auto satellite: satellites)
    {
        Serial.print("Id: " + String(satellite.id));
        Serial.print(", Name: " + String(satellite.name.c_str()));
        Serial.print(", Lng: " + String(satellite.coordinate.x));
        Serial.print(", Lat: " + String(satellite.coordinate.y));
        Serial.print(", Alt: " + String(satellite.altitude));
        Serial.println();
    }
    Serial.println();
}











