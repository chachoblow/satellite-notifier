#define ARDUINOJSON_DECODE_UNICODE 1
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1

#include <Arduino.h>
#include <WiFiMulti.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

struct Coordinates 
{
    double latitude;
    double longitude;
};

WiFiMulti wifiMulti;
WiFiClient client;
bool wifiConnected = false;

unsigned long lastConnectionTime = 0;
const unsigned long probeInterval = 5000L; // One minute.

String issCoordinatesAsString(Coordinates coordinates)
{
    return String(coordinates.latitude) + ", " + String(coordinates.longitude);
}

bool makeHttpRequest()
{
    client.println(F("GET /iss-now.json HTTP/1.0"));
    client.println(F("Host: api.open-notify.org"));
    client.println(F("Connection: close"));
    if (client.println() == 0)
    {
        Serial.println(F("Failed to send reqeust"));
        return false;
    }
    return true;
}

bool checkHttpStatus()
{
    char status[32] = { 0 };
    client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0)
    {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        return false;
    }
    return true;
}

bool skipHttpHeaders()
{
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders))
    {
        Serial.println(F("Invalid response"));
        return false;
    }
    return true;
}

Coordinates extractResponseValues(DynamicJsonDocument doc)
{
    String message = doc["message"];
    int timestamp = doc["timestamp"];
    double latitude = doc["iss_position"]["latitude"];
    double longitude = doc["iss_position"]["longitude"];
    Coordinates coordinates = { latitude, longitude };

    Serial.println(F("--- Response ---"));
    Serial.print(F("Message: "));
    Serial.println(message);
    Serial.print(F("Time stamp: "));
    Serial.println(timestamp);
    Serial.print(F("Coordinates: "));
    Serial.println(issCoordinatesAsString(coordinates));

    return coordinates;
}

Coordinates deserializeJson()
{
    const size_t capacity = 5 * (JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3));
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, client);
    if (error) 
    {
        Serial.print(F("deserializeJson() returned "));
        Serial.println(error.c_str());
        return {};
    }
    return extractResponseValues(doc);
}

Coordinates fetchIssCordinates() 
{
    client.stop();
    lastConnectionTime = millis();
    
    if (client.connect("api.open-notify.org", 80)) 
    {
        Serial.println("Connected to server.");
        Serial.println();
        if (!(makeHttpRequest() && checkHttpStatus() && skipHttpHeaders())) 
        {
            return {};
        }
        return deserializeJson();
    }
    else
    {
        Serial.println("Connection failed.");
        Serial.println();
        return {};
    }
}

void connectToWifi()
{
    // Connect to Wifi.
    wifiMulti.addAP("DucksBeakTheBears", "ChampKlein90");

    Serial.println();
    Serial.print("Connecting...");

    unsigned long wifiConnectionTime = millis();
    const unsigned long maxConnectionTime = 60000L;

    while (wifiMulti.run() != WL_CONNECTED && millis() - wifiConnectionTime < maxConnectionTime) 
    {
        delay(500);
    }

    if (wifiMulti.run() == WL_CONNECTED)
    {
        wifiConnected = true;
        Serial.println();
        Serial.print("Connected to ");
        Serial.println(WiFi.SSID());
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.println();
    }
    else 
    {
        Serial.println();
        Serial.println("Could not connect to WiFi.");
        Serial.println();
    }
}

void setup() 
{
    Serial.begin(9600);
    delay(100);
    connectToWifi();
}

void loop() 
{
    if (wifiConnected)
    {
        if (millis() - lastConnectionTime > probeInterval || lastConnectionTime == 0)
        {
            Coordinates coordinates = fetchIssCordinates();
            String coordinatesAsString = issCoordinatesAsString(coordinates);

            // TODO: Develop better way to error check. 
            if (coordinatesAsString.equals("0.00, 0.00"))
            {
                Serial.println("Error fetching coordinates: No coordinates returned.");
                Serial.println();
            }
            else
            {
                Serial.print("ISS coordinates: ");
                Serial.println(coordinatesAsString);
                Serial.println();
            }
        }
    }
}