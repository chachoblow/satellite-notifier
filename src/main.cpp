#define ARDUINOJSON_DECODE_UNICODE 1
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#define NUM_LEDS 16

#include <Arduino.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ShiftRegister74HC595.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
#include <SPI.h>
#include <Wire.h>

struct Coordinates 
{
    double latitude;
    double longitude;
};

WiFiMulti wifiMulti;
WiFiClientSecure client;
bool wifiConnected = false;

unsigned long lastConnectionTime = 0;
const unsigned long probeInterval = 5000L; // One minute.

const String API_KEY = "YGFU8B-QDQ8WZ-YDUHB5-4JEU";

int highestSatCount = 0;

int delayTime = 100;
int latchPin = 26; // RCLK
int clockPin = 27; // SRCLK
int dataPin = 25; // SER

// parameters: <number of shift registers> (data pin, clock pin, latch pin)
ShiftRegister74HC595<2> sr(dataPin, clockPin, latchPin);

Adafruit_IS31FL3731 ledMatrix = Adafruit_IS31FL3731();
uint8_t sweep[] = {1, 2, 3, 4, 6, 8, 10, 15, 20, 30, 40, 60, 60, 40, 30, 20, 15, 10, 8, 6, 4, 3, 2, 1};


bool makeHttpRequest()
{
    String myLat = "47.60621";
    String myLon = "-122.33207";
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

int extractResponseValues(DynamicJsonDocument doc)
{
    int satCount = doc["info"]["satcount"];

    Serial.println(F("--- Response ---"));
    Serial.print("Satellite count: ");
    Serial.println(satCount);

    highestSatCount = max(highestSatCount, satCount);

    Serial.println("Satellites:");
    for (int i = 0; i < satCount;  i++) 
    {
        String satName = doc["above"][i]["satname"];
        float satLat = doc["above"][i]["satlat"];
        float satLng = doc["above"][i]["satlng"];
        Serial.println("Name: " + satName + ", Lat: " + String(satLat) + ", Lng: " + String(satLng));
    }

    Serial.println();
    Serial.println("Highest sat count: " + String(highestSatCount));
    Serial.println();

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

void updateShiftRegister(byte storageByte)
{
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, storageByte);
    digitalWrite(latchPin, HIGH);
}

void setup() 
{
    Serial.begin(9600);
    delay(100);
    connectToWifi();

    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);

    if (!ledMatrix.begin()) 
  {
    Serial.println("ES31 not found.");
    while(1);
  }
  Serial.println("IS31 found.");
}

void updateSatCount()
{
    int satCount = fetchSatCount();

    if (satCount == -1)
    {
        Serial.println("Error fetching coordinates: No coordinates returned.");
        Serial.println();
    }
    else
    {
        Serial.print("Satellite count: ");
        Serial.println(satCount);
        Serial.println();
    }
}

void updateLeds()
{
    // byte storageByte = 0x1;
    // for (int i = 0; i < NUM_LEDS; i++) 
    // {
    //     updateShiftRegister(storageByte);
    //     storageByte = storageByte << 1;
    //     delay(delayTime);
    // }
    // for (int i = 0; i < NUM_LEDS - 1; i++) 
    // {
    //     updateShiftRegister(storageByte);
    //     storageByte = storageByte >> 1;
    //     delay(delayTime);
    // }

    sr.setAllHigh();
    delay(500);
    
    sr.setAllLow();
    delay(500);

    for (int i = 0; i < NUM_LEDS; i++) 
    {
        sr.set(i, HIGH);
        delay(250);
    }

    delay(500);
}

void loop() 
{
    if (wifiConnected && (millis() - lastConnectionTime > probeInterval || lastConnectionTime == 0))
    {
        updateSatCount();
    }

    //updateLeds();

    // animate over all the pixels, and set the brightness from the sweep table
  for (uint8_t incr = 0; incr < 24; incr++)
    for (uint8_t x = 0; x < 16; x++)
      for (uint8_t y = 0; y < 9; y++)
        ledMatrix.drawPixel(x, y, sweep[(x+y+incr)%24]);
  delay(20);
}