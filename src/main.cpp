#include <time.h>
#include <WiFi.h>

#include "header.h"

void setup()
{

    Serial.begin(115200);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected.");

    configTime(gmtOffset, daylightOffset, ntpServer);

    u8g2.begin();

    pinMode(nextButton, INPUT_PULLUP);
    pinMode(prevButton, INPUT_PULLUP);
    pinMode(selectButton, INPUT_PULLUP);
}

void loop()
{

    display(displayPage);
    delay(100);
}
