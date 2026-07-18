#include <time.h>

#include "header.h"

void setup()
{

    Serial.begin(115200);

    // Connect to WiFi
    startWifi();

    // Set up LittleFS
    startLittleFS();

    // Load certificates
    loadCerts();
    // Set up Transport Layer Security
    setupTLS();

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
