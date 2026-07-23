#include <time.h>

#include "header.h"

void setup()
{

    Serial.begin(115200);

    // Connect to WiFi
    startWifi();

    configTime(gmtOffset, daylightOffset, ntpServer);
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
    if (startTime - lastPublish >= PUBLISH_INTERVAL)
    {
        lastPublish = startTime;
        publishData(getActualTemp(), getFeelsLikeTemp());
    }
    display(displayPage);
    delay(100);
}
