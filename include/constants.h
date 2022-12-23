#pragma once

namespace LedMatrixConstants
{
    const int BOARD_WIDTH = 16;
    const int BOARD_HEIGHT = 9;
}

namespace SatelliteConstants
{
    const float MY_LAT = 47.60621;
    const float MY_LNG = -122.33207;
    const float MY_ALT = 0;
    const int SAT_CATEGORY_ID = 0;
    const float SEARCH_RADIUS = 7;
}

namespace N2YORequestConstants
{
    const String API_KEY = "YGFU8B-QDQ8WZ-YDUHB5-4JEU";
    const unsigned long PROBE_INTERVAL = 5000L;
}

namespace SpeakerConstants
{
    const uint8_t CHANNEL = 0;
    const uint8_t BITS = 8;
    const uint8_t PIN = 23;
    const int MAX_VALUES = 256;
    const float CONVERSION_FACTOR = (2 * PI) / MAX_VALUES;
}