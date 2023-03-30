#pragma once

#include <QMC5883LCompass.h>

class Compass
{
public:
    Compass();
    void initialize();
    int bearing();

private:
    QMC5883LCompass _compass;
};