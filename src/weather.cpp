#include <ArduinoJson.h>
#include <cstring>
#include <HTTPClient.h>

#include "header.h"

Weather getWeather()
{
    // Verify WiFi connection
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected, skipping fetch.");
        return cachedWeather;
    }

    // Check time since laast update
    if (millis() - lastFetchTime < fetchInterval && lastFetchTime != 0)
    {
        return cachedWeather;
    }

    Weather weather = {-1, -1, -1, " ", " "};

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
            return weather;
        }
        weather.actualTemp = doc["main"]["temp"];
        weather.feelsLikeTemp = doc["main"]["feels_like"];
        weather.humidity = doc["main"]["humidity"];

        // Null pointer guard
        const char *description = doc["weather"][0]["description"] | "";
        const char *icon = doc["weather"][0]["icon"] | "";
        std::snprintf(weather.weather, sizeof(weather.weather), "%s", description);
        std::snprintf(weather.weatherIcon, sizeof(weather.weatherIcon), "%s", icon);
    }
    else
    {
        Serial.printf("Request failed, code: %d\n", httpCode);
    }
    http.end();

    cachedWeather = weather;
    lastFetchTime = millis();
    return cachedWeather;
}

// ---------------------------------Page 1----------------------------------
void currentTemperatures()
{
    u8g2.clearBuffer();
    Weather weather = getWeather();

    // -------------------------Actual temperature-------------------------
    u8g2.setFont(u8g2_font_6x10_tf);
    const char *actualTempText = "Temperature:";
    int actualTempTextPos = u8g2.getStrWidth(actualTempText);
    u8g2.setCursor((128 - actualTempTextPos) / 2, 7);
    u8g2.print(actualTempText);

    u8g2.setFont(u8g2_font_fub17_tf);
    char actualTemp[7];
    snprintf(actualTemp, sizeof(actualTemp), "%.2f", weather.actualTemp);
    int actualTempPos = u8g2.getStrWidth(actualTemp);
    int actualUnitSymbolPos = (128 - actualTempPos) / 2 + actualTempPos;
    u8g2.setCursor((128 - actualTempPos - 12) / 2, positions[1]);
    u8g2.print(actualTemp);

    // Unit symbol
    u8g2.drawCircle(actualUnitSymbolPos - 2, positions[0] + 3, 2);
    u8g2.setCursor(actualUnitSymbolPos + 1, positions[1]);
    u8g2.print("C");

    // ------------------------Feels like temperature------------------------
    u8g2.setFont(u8g2_font_6x10_tf);
    const char *feelsLikeTempText = "Feels like: ";
    int feelsLikeTempPos = u8g2.getStrWidth(feelsLikeTempText);
    char feelsLikeTempbuffer[7];
    snprintf(feelsLikeTempbuffer, sizeof(feelsLikeTempbuffer), "%.2f", weather.feelsLikeTemp);
    int feelsLikeUnitSymbolPos = feelsLikeTempPos + u8g2.getStrWidth(feelsLikeTempbuffer) + 2;
    u8g2.setCursor(0, positions[2]);
    u8g2.print(feelsLikeTempText);

    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.setCursor(feelsLikeTempPos, positions[2]);
    u8g2.print(feelsLikeTempbuffer);

    // Unit symbol
    u8g2.drawCircle(feelsLikeUnitSymbolPos + 1, positions[2] - 7, 1);
    u8g2.drawStr(feelsLikeUnitSymbolPos + 4, positions[2], "C");
}

void displayWeather()
{
    selectedFeaturePage = 1;
    totalFeaturePages = 1;
    // Plan to add option to use external hardware to get humidity and tempertaure
    // and defaulting to external API when external hardware isn't present
    currentTemperatures();

    // Indicator showing selected page in app
    int x = 50;
    for (int i = 1; i <= 4; i++)
    {
        if (i == selectedFeaturePage)
        {
            u8g2.drawDisc(x, 56, 2);
        }
        else
        {
            u8g2.drawCircle(x, 56, 2);
        }
        x += 9;
    }
    if (!nextButton && !isNextDebouncing && selectedFeaturePage != totalFeaturePages)
    {
        selectedFeaturePage++;
    }
    if (!prevButton && !isPrevDebouncing && selectedFeaturePage != 1)
    {
        selectedFeaturePage--;
    }
    if (!digitalRead(selectButton) && !isSelectDebouncing)
    {
        displayPage = 1;
    }
    nextDelay();
    prevDelay();
    selectDelay();
    u8g2.sendBuffer();
}
