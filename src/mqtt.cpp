#include "header.h"

void connectAWS()
{
    client.setServer("192.168.31.101", 1883);
    while (!client.connected())
    {
        if (client.connect(THING_NAME))
        {
            Serial.println("Connected!");
        }
        else
        {
            delay(2000);
        }
    }
}

void publishData(float actualTemp, float feelsLikeTemp)
{

    JsonDocument jsonDoc;
    jsonDoc["actualTemp"] = actualTemp;
    jsonDoc["feelsLikeTemp"] = feelsLikeTemp;
    jsonDoc["timestamp"] = millis();

    char buffer[128];
    size_t n = serializeJson(jsonDoc, buffer);

    bool ok = client.publish("esp32/pub", buffer, n);
    Serial.println(ok ? "Published" : "Publish failed");
}