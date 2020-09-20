#define ARDUINOJSON_DECODE_UNICODE 1
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#define NUM_LEDS 16

#include <Arduino.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
#include <SPI.h>
#include <Wire.h>

#include "coordinate.h"
#include "led.h"

WiFiMulti wifiMulti;
WiFiClientSecure client;
bool wifiConnected = false;

unsigned long lastConnectionTime = 0;
const unsigned long probeInterval = 5000L;

const String API_KEY = "YGFU8B-QDQ8WZ-YDUHB5-4JEU";

Adafruit_IS31FL3731 ledMatrix = Adafruit_IS31FL3731();

const float LEDS_IN_ROW = 9;
const float RADIUS = 15;
const float LED_BLOCK_SIZE = (RADIUS * 2) / LEDS_IN_ROW;

const float CENTER_LAT = 47.60621;
const float CENTER_LNG = -122.33207;
const float MIN_LAT = CENTER_LAT - RADIUS;
const float MAX_LAT = CENTER_LAT + RADIUS;
const float MIN_LNG = CENTER_LNG - RADIUS;
const float MAX_LNG = CENTER_LNG + RADIUS;

const int ARRAY_SIZE = 30;

Coordinate satellites[ARRAY_SIZE];
Led leds[ARRAY_SIZE];
int satelliteCount = 0;
int ledCount = 0;

int extractResponseValues(DynamicJsonDocument doc)
{
    int satCount = doc["info"]["satcount"];

    Serial.println(F("--- Response ---"));
    Serial.print("Satellite count: ");
    Serial.println(satCount);

    for (int i = 0; i < satCount;  i++) 
    {
        String satName = doc["above"][i]["satname"];

        if (satName.indexOf("DEB") < 0)
        {
            float satLat = doc["above"][i]["satlat"];
            float satLng = doc["above"][i]["satlng"];
            
            Serial.println("Name: " + satName + ", Lat: " + String(satLat) + ", Lng: " + String(satLng));

            Coordinate current(satLat, satLng);
            satellites[i] = current;
        }
    }

    return satCount;
}

int deserializeJson()
{
    StaticJsonDocument<256> filter;
    filter["info"]["satcount"] = true;
    filter["above"][0]["satname"] = true;
    filter["above"][0]["satlat"] = true;
    filter["above"][0]["satlng"] = true;

    // Enough space for up to 40 satellites (could probably trim down if needed).
    const size_t capacity = JSON_ARRAY_SIZE(100) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 100 * JSON_OBJECT_SIZE(3);
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, client, DeserializationOption::Filter(filter));
    if (error) 
    {
        Serial.print(F("deserializeJson() returned: "));
        Serial.println(error.c_str());
        return -1;
    }
    return extractResponseValues(doc);
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

bool makeHttpRequest()
{
    String myLat = String(CENTER_LAT);
    String myLon = String(CENTER_LNG);
    String myAlt = "0";
    String searchRadius = "15"; 
    String categoryId = "0";
    client.println("GET /rest/v1/satellite/above/" + myLat + "/" + myLon + "/" + myAlt + "/" + searchRadius + "/" + categoryId + "/&apiKey=" + API_KEY + " HTTP/1.0");
    client.println(F("Host: www.n2yo.com"));
    client.println(F("Connection: close"));
    if (client.println() == 0)
    {
        Serial.println(F("Failed to send reqeust"));
        return false;
    }
    return true;
}

int fetchSatCount() 
{
    client.stop();
    lastConnectionTime = millis();
    
    if (client.connect("www.n2yo.com", 443)) 
    {
        Serial.println("Connected to server.");
        Serial.println();
        if (!(makeHttpRequest() && checkHttpStatus() && skipHttpHeaders())) 
        {
            return -1;
        }
        return deserializeJson();
    }
    else
    {
        Serial.println("Connection failed.");
        Serial.println();
        return -1;
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

    if (!ledMatrix.begin()) 
    {
        Serial.println("ES31 not found.");
        while(1);
    }

    Serial.println("IS31 found.");
}

boolean satInBounds(Coordinate sat)
{
    return sat.x >= MIN_LAT && sat.x <= MAX_LAT && sat.y >= MIN_LNG && sat.y <= MAX_LNG;
}

Coordinate buildLedCoordinate(Coordinate satellite)
{
    float latDiff = satellite.x - MIN_LAT;
    float lngDiff = satellite.y - MIN_LNG;
    int latRow = latDiff / LED_BLOCK_SIZE;
    int lngRow = lngDiff / LED_BLOCK_SIZE;

    Coordinate coordinate(latRow, lngRow);
    return coordinate;
}

int getInBoundsCoordinates(Coordinate *inBoundsCoordinates)
{
    int inBoundsCount = 0;

    for (int i = 0; i < satelliteCount; i++)
    {
        if (satInBounds(satellites[i])) 
        {
            Coordinate coordinate = buildLedCoordinate(satellites[i]);
            inBoundsCoordinates[inBoundsCount++] = coordinate;
        }
    }

    return inBoundsCount;
}

int getUniqueLeds(Led *uniqueLeds, Coordinate *getInBoundsCoordinates, int inBoundsCount)
{
    int uniqueLedCount = 0;

    for (int i = 0; i < inBoundsCount; i++)
    {
        Coordinate current = getInBoundsCoordinates[i];
        bool isUniqueLed = true;

        for (int j = 0; j < uniqueLedCount; j++)
        {
            if (current.equals(uniqueLeds[j].coordinate))
            {
                uniqueLeds[j].count++;
                isUniqueLed = false;
                break;
            }
        }

        if (isUniqueLed)
        {
            Led uniqueLed(current, 1);
            uniqueLeds[uniqueLedCount++] = uniqueLed;
        }
    }

    return uniqueLedCount;
}

void clearEmptyLeds(Led *uniqueLeds, int uniqueLedCount)
{
    for (int i = 0; i < ledCount; i++)
    {
        Led led = leds[i];
        bool stillDisplayed = false;

        for (int j = 0; j < uniqueLedCount; j++)
        {
            if (led.coordinate.equals(uniqueLeds[j].coordinate))
            {
                stillDisplayed = true;
                break;
            }
        }

        if (!stillDisplayed)
        {
            Coordinate coordinate = leds[i].coordinate;
            ledMatrix.writePixel(coordinate.x, coordinate.y, 0);
        }
    }
}

void writeLeds(Led *uniqueLeds, int uniqueLedCount)
{
    for (int i = 0; i < uniqueLedCount; i++)
    {
        Coordinate coordinate = uniqueLeds[i].coordinate;
        ledMatrix.writePixel(coordinate.x, coordinate.y, 255);
        leds[i] = uniqueLeds[i];
    }

    ledCount = uniqueLedCount;
}

void printActiveLeds()
{
    Serial.println();
    Serial.println("--- Active LEDs ---");
    for (int i = 0; i < ledCount; i++)
    {
        Led current = leds[i];
        Serial.println("Count: " + String(current.count) + " x: " + String(current.coordinate.x) + " y: " + String(current.coordinate.y));
    }
    Serial.println();
}

void updateLeds()
{
    Coordinate inBoundsCoordinates[ARRAY_SIZE];
    int inBoundsCount = getInBoundsCoordinates(inBoundsCoordinates);

    Led uniqueLeds[ARRAY_SIZE];
    int uniqueLedCount = getUniqueLeds(uniqueLeds, inBoundsCoordinates, inBoundsCount);

    clearEmptyLeds(uniqueLeds, uniqueLedCount);
    writeLeds(uniqueLeds, uniqueLedCount);
    printActiveLeds();
}

void loop() 
{
    if (wifiConnected && (millis() - lastConnectionTime > probeInterval || lastConnectionTime == 0))
    {
        satelliteCount = fetchSatCount();
        if (satelliteCount != -1) 
        {
            updateLeds();
        }
    }
}