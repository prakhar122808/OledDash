#include <time.h>

#include "header.h"

void waitForTime()
{
    time_t now = time(nullptr);
    Serial.print("Waiting for NTP sync");
    while (now < 1700000000)
    { // sanity threshold, any time after ~Nov 2023
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("\nTime synced: " + String(now));
}

void setup()
{

    Serial.begin(115200);

    // Connect to WiFi
    startWifi();

    configTime(gmtOffset, daylightOffset, ntpServer);
    waitForTime();
    u8g2.begin();

    pinMode(nextButton, INPUT_PULLUP);
    pinMode(prevButton, INPUT_PULLUP);
    pinMode(selectButton, INPUT_PULLUP);
}

void loop()
{
    if (!client.connected())
    {
        connectAWS();
    }
    client.loop();
    unsigned long now = millis();
    if (now - lastPublish >= PUBLISH_INTERVAL)
    {
        lastPublish = now;
        publishData(getActualTemp(), getFeelsLikeTemp());
    }
    display(displayPage);
    delay(100);
}
