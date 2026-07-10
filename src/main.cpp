#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <time.h>
#include <U8g2lib.h>
#include <WiFi.h>

#include "credentials.h"

// U8g2 object for SH1106 128x64 OLED display using hardware I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// WiFi credentials and NTP server configuration
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *ntpServer = "in.pool.ntp.org";
const long gmtOffset = 19800; // +5:30(IST)
const long daylightOffset = 0;

// Button pin definitions
#define upButton 16
#define downButton 17
#define selectButton 18

// Menu options and positions
int positions[] = {16, 32, 48, 64};
const char *mainMenu[] = {"Date and Time", "Weather"};
const int numFeatures = sizeof(mainMenu) / sizeof(mainMenu[0]);

// Variables to track current selection and page
int currentPage = 0;
int displayPage = 1;

// Variables related to weather API
struct Temperatures
{
    float actualTemp;
    float feelsLikeTemp;
};
Temperatures cachedTemps = {-1, -1};
unsigned long lastFetchTime = 0;
const unsigned long fetchInterval = 60000; // 1 minute

// Function declarations
void displayStartingMenu();
void displayTime(char date[], char time[]);
void displayWeather();
void display(int displayPage, char date[], char time[]);
void enableNavigation();
void menuDateTimePage();
Temperatures getTemps();

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
    u8g2.setFont(u8g2_font_ncenB08_tr);

    pinMode(upButton, INPUT_PULLUP);
    pinMode(downButton, INPUT_PULLUP);
    pinMode(selectButton, INPUT_PULLUP);
}

void loop()
{

    // Get the current date and time
    struct tm timeInfo;

    if (!getLocalTime(&timeInfo))
    {
        Serial.println("Failed to obtain time.");
        return;
    }

    // Format the date and time into strings
    char dateStr[32] = {0};
    char timeStr[32] = {0};
    strftime(dateStr, sizeof(dateStr), "%A, %B %d %Y", &timeInfo);
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeInfo);
    int count = 0;
    int timeCount = 0;
    int gapsBetweenDateAndTime = 4;

    display(displayPage, dateStr, timeStr);
    delay(100);
}

void displayStartingMenu()
{
    u8g2.clearBuffer();
    enableNavigation();
    for (int pos = 0; pos < numFeatures; pos++)
    {
        if (currentPage == pos)
        {
            u8g2.setCursor(0, positions[pos]);
            u8g2.print('>');
        }
        u8g2.setCursor(10, positions[pos]);
        u8g2.print(mainMenu[pos]);
    }

    if (!digitalRead(selectButton))
    {
        switch (currentPage)
        {
        case 0:
        {
            displayPage = 2;
            break;
        }
        case 1:
        {
            displayPage = 3;
            break;
        }
        }
        delay(30);
    }
    u8g2.sendBuffer();
}

void displayTime(char dateStr[], char timeStr[])
{

    u8g2.clearBuffer();
    u8g2.setCursor(0, positions[0]);
    u8g2.print(dateStr);
    u8g2.setCursor(0, positions[1]);
    u8g2.print(timeStr);
    u8g2.setCursor(0, positions[2]);
    u8g2.print("Press Select to go back");
    if (!digitalRead(selectButton))
    {
        displayPage = 1;
    }
    delay(30);
    u8g2.sendBuffer();
}

void displayWeather()
{

    u8g2.clearBuffer();
    Temperatures temperatures = getTemps();
    // Actual temperature
    const char *actualTempText = "Temperature: ";
    int actualTempPos = u8g2.getStrWidth(actualTempText);

    u8g2.setCursor(0, positions[0]);
    u8g2.print(actualTempText);
    u8g2.setCursor(actualTempPos, positions[0]);
    u8g2.print(temperatures.actualTemp);

    // Feels like temperature
    const char *feelsLikeTempText = "Feels like: ";
    int feelsLikeTempPos = u8g2.getStrWidth(feelsLikeTempText);

    u8g2.setCursor(0, positions[1]);
    u8g2.print(feelsLikeTempText);
    u8g2.setCursor(feelsLikeTempPos, positions[1]);
    u8g2.print(temperatures.feelsLikeTemp);

    u8g2.setCursor(0, positions[2]);
    u8g2.print("Press Select to go back");
    if (!digitalRead(selectButton))
    {
        displayPage = 1;
    }
    delay(30);
    u8g2.sendBuffer();
}

void display(int displayPage, char dateStr[], char timeStr[])
{
    switch (displayPage)
    {
    case 1:
        displayStartingMenu();
        break;
    case 2:
        displayTime(dateStr, timeStr);
        break;
    case 3:
        displayWeather();
        break;
    }
}

void enableNavigation()
{
    if (!digitalRead(upButton))
    {
        currentPage--;
        if (currentPage < 0)
            currentPage += numFeatures;
        delay(30);
    }
    if (!digitalRead(downButton))
    {
        currentPage++;
        currentPage %= numFeatures;
        delay(30);
    }
}

Temperatures getTemps()
{
    // Verify WiFi connection
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected, skipping fetch.");
        return cachedTemps; // or temps = {-1,-1} if no cache yet
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