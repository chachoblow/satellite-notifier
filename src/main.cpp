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

struct Coordinates
{
    int x;
    int y;
};

struct LedInfo
{
    Coordinates coordinates;
    int count;
};

WiFiMulti wifiMulti;
WiFiClientSecure client;
bool wifiConnected = false;

unsigned long lastConnectionTime = 0;
const unsigned long probeInterval = 3600L;

const String API_KEY = "YGFU8B-QDQ8WZ-YDUHB5-4JEU";

Adafruit_IS31FL3731 ledMatrix = Adafruit_IS31FL3731();

const float LEDS_IN_ROW = 9;
const float RADIUS = 5;
const float LED_BLOCK_SIZE = (RADIUS * 2) / LEDS_IN_ROW;

const float CENTER_LAT = 47.677490;
const float CENTER_LNG = -122.265210;
const float MIN_LAT = CENTER_LAT - RADIUS;
const float MAX_LAT = CENTER_LAT + RADIUS;
const float MIN_LNG = CENTER_LNG - RADIUS;
const float MAX_LNG = CENTER_LNG + RADIUS;

const int ARRAY_SIZE = 30;

Coordinates satCoordinates[ARRAY_SIZE];
LedInfo leds[ARRAY_SIZE];
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
        float satLat = doc["above"][i]["satlat"];
        float satLng = doc["above"][i]["satlng"];
        
        Serial.println("Name: " + satName + ", Lat: " + String(satLat) + ", Lng: " + String(satLng));

        Coordinates current;
        current.x = satLat;
        current.y = satLng;
        satCoordinates[i] = current;
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
    const size_t capacity = JSON_ARRAY_SIZE(40) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 40 * JSON_OBJECT_SIZE(3);
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
    String myLat = "47.677490";
    String myLon = "-122.265210";
    String myAlt = "0";
    String searchRadius = "7"; 
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

boolean coordinatesEqual(Coordinates a, Coordinates b)
{
    return a.x == b.x && a.y == b.y;
}

boolean satInBounds(Coordinates sat)
{
    return sat.x >= MIN_LAT && sat.x <= MAX_LAT && sat.y >= MIN_LNG && sat.y <= MAX_LNG;
}

Coordinates buildLedCoordinates(Coordinates satCoordinates)
{
    float latDiff = satCoordinates.x - MIN_LAT;
    float lngDiff = satCoordinates.y - MIN_LNG;
    int latRow = latDiff / LED_BLOCK_SIZE;
    int lngRow = lngDiff / LED_BLOCK_SIZE;

    Coordinates coordinates;

    coordinates.x = latRow;
    coordinates.y = lngRow;

    return coordinates;
}

int getInBoundsCoordinates(Coordinates *inBoundsCoordinates)
{
    int inBoundsCount = 0;

    for (int i = 0; i < satelliteCount; i++)
    {
        if (satInBounds(satCoordinates[i])) 
        {
            Coordinates ledCoordinates = buildLedCoordinates(satCoordinates[i]);
            inBoundsCoordinates[inBoundsCount++] = ledCoordinates;
        }
    }

    return inBoundsCount;
}

int getUniqueLeds(LedInfo *uniqueLeds, Coordinates *getInBoundsCoordinates, int inBoundsCount)
{
    int uniqueLedCount = 0;

    for (int i = 0; i < inBoundsCount; i++)
    {
        Coordinates current = getInBoundsCoordinates[i];
        bool isUniqueLed = true;

        for (int j = 0; j < uniqueLedCount; j++)
        {
            if (coordinatesEqual(current, uniqueLeds[j].coordinates))
            {
                uniqueLeds[j].count++;
                isUniqueLed = false;
                break;
            }
        }

        if (isUniqueLed)
        {
            LedInfo uniqueLed;
            uniqueLed.count = 1;
            Coordinates coordinates;
            coordinates.x = current.x;
            coordinates.y = current.y;
            uniqueLed.coordinates = coordinates;
            uniqueLeds[uniqueLedCount++] = uniqueLed;
        }
    }

    return uniqueLedCount;
}

void clearEmptyLeds(LedInfo *uniqueLeds, int uniqueLedCount)
{
    for (int i = 0; i < ledCount; i++)
    {
        LedInfo currentLed = leds[i];
        bool stillDisplayed = false;

        for (int j = 0; j < uniqueLedCount; j++)
        {
            if (coordinatesEqual(uniqueLeds[j].coordinates, currentLed.coordinates))
            {
                stillDisplayed = true;
                break;
            }
        }

        if (!stillDisplayed)
        {
            ledMatrix.writePixel(leds[i].coordinates.x, leds[i].coordinates.y, 0);
        }
    }
}

void writeLeds(LedInfo *uniqueLeds, int uniqueLedCount)
{
    for (int i = 0; i < uniqueLedCount; i++)
    {
        Coordinates uniqueCoordinates = uniqueLeds[i].coordinates;
        ledMatrix.writePixel(uniqueCoordinates.x, uniqueCoordinates.y, 255);
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
        LedInfo current = leds[i];
        Serial.println("Count: " + String(current.count) + " x: " + String(current.coordinates.x) + " y: " + String(current.coordinates.y));
    }
    Serial.println();
}

void updateLeds()
{
    Coordinates inBoundsCoordinates[ARRAY_SIZE];
    int inBoundsCount = getInBoundsCoordinates(inBoundsCoordinates);

    LedInfo uniqueLeds[ARRAY_SIZE];
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