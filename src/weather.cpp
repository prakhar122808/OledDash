#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "header.h"

Temperatures getTemps()
{
    // Verify WiFi connection
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected, skipping fetch.");
        return cachedTemps;
    }

    // Check time since laast update
    if (millis() - lastFetchTime < fetchInterval && lastFetchTime != 0)
    {
        return cachedTemps;
    }

    Temperatures temps = {-1, -1};

    HTTPClient http;
    String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(lat, 7) + "&lon=" + String(lon, 7) + "&appid=" + API_KEY + "&units=metric";

    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        String payload = http.getString();

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("Failed to parse JSON data.");
            Serial.println(error.c_str());
            http.end();
            return temps;
        }
        temps.actualTemp = doc["main"]["temp"];
        temps.feelsLikeTemp = doc["main"]["feels_like"];
    }
    else
    {
        Serial.printf("Request failed, code: %d\n", httpCode);
    }
    http.end();

    cachedTemps = temps;
    lastFetchTime = millis();
    return cachedTemps;
}

void displayWeather()
{
    // Plan to add option to use external hardware to get humidity and tempertaure
    // and defaulting to external API when external hardware isn't present

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_t0_11b_tf);
    Temperatures temperatures = getTemps();

    // -------------------------Actual temperature-------------------------
    const char *actualTempText = "Temperature: ";
    int actualTempPos = u8g2.getStrWidth(actualTempText);
    char actualTempbuffer[7];
    snprintf(actualTempbuffer, sizeof(actualTempbuffer), "%.2f", temperatures.actualTemp);
    int actualunitSymbolPos = actualTempPos + u8g2.getStrWidth(actualTempbuffer) + 2;

    u8g2.setCursor(0, positions[0]);
    u8g2.print(actualTempText);
    u8g2.setCursor(actualTempPos, positions[0]);
    u8g2.print(actualTempbuffer);

    // Unit symbol
    u8g2.drawCircle(actualunitSymbolPos, positions[0] - 7, 1);
    u8g2.drawStr(actualunitSymbolPos + 3, positions[0], "C");

    // ------------------------Feels like temperature------------------------
    const char *feelsLikeTempText = "Feels like: ";
    int feelsLikeTempPos = u8g2.getStrWidth(feelsLikeTempText);
    char feelsLikeTempbuffer[7];
    snprintf(feelsLikeTempbuffer, sizeof(feelsLikeTempbuffer), "%.2f", temperatures.feelsLikeTemp);
    int feelsLikeUnitSymbolPos = feelsLikeTempPos + u8g2.getStrWidth(feelsLikeTempbuffer) + 2;

    u8g2.setCursor(0, positions[1]);
    u8g2.print(feelsLikeTempText);
    u8g2.setCursor(feelsLikeTempPos, positions[1]);
    u8g2.print(feelsLikeTempbuffer);

    // Unit symbol
    u8g2.drawCircle(feelsLikeUnitSymbolPos, positions[1] - 7, 1);
    u8g2.drawStr(feelsLikeUnitSymbolPos + 3, positions[1], "C");

    u8g2.setCursor(0, positions[2]);
    u8g2.print("Select to go back");
    if (!digitalRead(selectButton) && !isSelectDebouncing)
    {
        displayPage = 1;
    }
    selectDelay();
    u8g2.sendBuffer();
}
