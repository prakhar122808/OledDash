#pragma once
#include <Arduino.h>

class NonBlockingDelay
{

private:
    unsigned long previousMillis;
    unsigned long interval;
    bool isActive;

public:
    NonBlockingDelay(unsigned long ms = 1000)
    {
        interval = ms;
        previousMillis = 0;
        isActive = false;
    }

    void setInterval(unsigned long ms)
    {
        interval = ms;
    }

    void start()
    {
        previousMillis = millis();
        isActive = true;
    }

    void stop()
    {
        isActive = false;
    }

    bool isElapsed()
    {
        if (!isActive)
            return false;
        if (millis() - previousMillis >= interval)
        {
            isActive = false;
            return true;
        }
        return false;
    }

    bool check()
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
            previousMillis = currentMillis;
            return true;
        }
        return false;
    }
};
