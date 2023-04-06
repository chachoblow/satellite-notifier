#include <speaker.h>

Speaker::Speaker() = default;

Speaker::Speaker(uint8_t channel, uint8_t bits, uint8_t pin)
{
    _channel = channel;
    _bits = bits;
    _pin = pin;
    _counter = 200;
    _step = 1;
}

void Speaker::initialize()
{
    Serial.println("Initializing speaker...");
    Serial.println();

    ledcSetup(_channel, 0, _bits);
    ledcAttachPin(_pin, _channel);

    for (int i = 0; i < SpeakerConstants::MAX_VALUES; i++)
    {
        int angle = i * SpeakerConstants::CONVERSION_FACTOR;
        sinValues.push_back((sin(angle) * 127) + 128);
    }
}

void Speaker::player(std::vector<Satellite> &satellites)
{
    int minCounter = 200;
    int maxCounter = 400;

    if (_counter < minCounter)
    {
        _step = 1;
    }
    else if (_counter > maxCounter)
    {
        _step = -1;
    }

    _counter += _step;

    Serial.println("--- Satellites in player ---");

    std::vector<double> frequencies;
    std::vector<double> magnitudes;
    std::vector<double> signals;

    for (int i = 0; i < satellites.size(); i++)
    {
        Serial.print("Altitude: " + String(satellites[i].altitude));
        Serial.print(", Coordinates: " + String(satellites[i].latitude) + " " + String(satellites[i].longitude));
        Serial.println(", Name: " + String(satellites[i].name.c_str()));
        float frequency = (abs(satellites[i].latitude) + abs(satellites[i].longitude));
        frequency = map(frequency, 0, 250, 50, 1000);
        frequencies.push_back(frequency);
        float constrainedAlt = constrain(satellites[i].altitude, 1000, 30000);
        float magnitude = map(constrainedAlt, 1000, 30000, 50, 1000);
        magnitudes.push_back(magnitude);
    }

    for (int i = 0; i < satellites.size(); i++)
    {
        float signal = magnitudes[i] * sin(frequencies[i] + _counter) + frequencies[i];
        signal = constrain(signal, 0, 1000);
        signal = map(signal, 0, 1000, 5, 175);
        signals.push_back(abs(signal));
    }

    Serial.println("--- Sin values ---");

    for (int i = 0; i < satellites.size() - 1; i++)
    {
        Serial.print("Frequency: " + String(frequencies[i]));
        Serial.print(", Magnitude: " + String(magnitudes[i]));
        Serial.println(", Signal: " + String(signals[i]));
    }

    uint32_t duty = 5;
    for (int i = 0; i < 25; i++)
    {
        Serial.println("Iteration: " + String(i));
        for (int j = 0; j < satellites.size(); j++)
        {
            if (j == 0)
            {
                Speaker::SmoothStep(signals[satellites.size() - 1], signals[0], 25, duty);
            }
            else
            {
                Speaker::SmoothStep(signals[j - 1], signals[j], 25, duty);
            }
        }
    }

    ledcWriteTone(_channel, 0);
}

void Speaker::SmoothStep(float min, float max, float numSteps, uint32_t duty)
{
    float x;
    float v;

    uint32_t cycleDuty = 51;
    uint32_t dutyStep = 2;

    int delayTime = 6;
    int delayStep = 1;

    for (int i = 0; i < numSteps; i++)
    {
        v = i / numSteps;
        v = Speaker::SmoothStepIncrement(v);
        x = (max * v) + (min * (1 - v));

        if (cycleDuty <= 50 || cycleDuty >= 150)
        {
            dutyStep *= -1;
        }

        if (delayTime <= 5 || delayTime >= 10)
        {
            delayStep *= -1;
        }

        cycleDuty += dutyStep;
        delayTime += delayStep;

        ledcWriteTone(_channel, x);
        ledcWrite(_channel, cycleDuty);
        delay(4);
    }
}

float Speaker::SmoothStepIncrement(float x)
{
    float increment = x * x * (3 - (2 * x));
    // 20 sounded pretty cool...5 is a bit smoother with a similar effect.
    return pow(increment, 20);
}

float Speaker::catmullrom(float t, float p0, float p1, float p2, float p3)
{
    float increment = 0.5f * ((2 * p1) +
                              (-p0 + p2) * t +
                              (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t +
                              (-p0 + 3 * p1 - 3 * p2 + p3) * t * t * t);

    return pow(increment, 10);
}