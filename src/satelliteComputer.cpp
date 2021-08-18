#include "satelliteComputer.h"
#include <ArduinoJson.h>

SatelliteComputer::SatelliteComputer()
{
    WiFiClientSecure _client;
    lastConnectionTime = 0;
}

int SatelliteComputer::fetchSatellites(std::vector<Satellite> &satellites) 
{
    _client.stop();

    Serial.println("Connecting to satellite service.");

    _client.setInsecure();
    // TODO: Get the certificate working and don't use insecure connection.
    // const char* test_root_ca = \
    // "-----BEGIN CERTIFICATE-----\n" \
    // "MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\n" \
    // "iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n" \
    // "cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n" \
    // "BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\n" \
    // "MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\n" \
    // "BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\n" \
    // "aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\n" \
    // "dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\n" \
    // "AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\n" \
    // "3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\n" \
    // "tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\n" \
    // "Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\n" \
    // "VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\n" \
    // "79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\n" \
    // "c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\n" \
    // "Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\n" \
    // "c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\n" \
    // "UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\n" \
    // "Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\n" \
    // "BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\n" \
    // "A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\n" \
    // "Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\n" \
    // "VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\n" \
    // "ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\n" \
    // "8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\n" \
    // "iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\n" \
    // "Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\n" \
    // "XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\n" \
    // "qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\n" \
    // "VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\n" \
    // "L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\n" \
    // "jjxDah2nGN59PRbxYvnKkKj9\n" \
    // "-----END CERTIFICATE-----\n" ;
    // _client.setCACert(test_root_ca);
    
    if (_client.connect("api.n2yo.com", 443)) 
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











