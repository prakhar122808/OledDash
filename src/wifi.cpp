#include "header.h"

// Initialise WiFi connection
void startWifi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected.");
}

void setupTLS()
{
    net.setCACert(rootCA.c_str());
    net.setCertificate(deviceCert.c_str());
    net.setPrivateKey(privateKey.c_str());
}