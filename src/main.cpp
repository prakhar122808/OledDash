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
#define numOptions 4

// Menu options and positions
int positions[] = {16, 32, 48, 64};
const char *pageOneOptions[] = {"Date and Time", "Weather", "Option 3", "Option 4"};

// Variables to track current selection and page
int currentSelection = 0;
int pageSelection = 1;

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
void displayStartingMenuOptionOne(char date[], char time[]);
void displayStartingMenuOptionTwo();
void displayStartingMenuOptionThree();
void displayStartingMenuOptionFour();
void display(int pageSelection, char date[], char time[]);
void enableNavigation();
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

    // Get the current time
    struct tm timeInfo;

    if (!getLocalTime(&timeInfo))
    {
        Serial.println("Failed to obtain time.");
        return;
    }

    // Format the date and time into strings
    char dateTimeStr[64];
    strftime(dateTimeStr, sizeof(dateTimeStr), "%A, %B %d %Y %H:%M:%S", &timeInfo); // Time starts after 4 spaces from the date
    char date[32] = {0};
    char time[32] = {0};
    int count = 0;
    int timeCount = 0;
    int gapsBetweenDateAndTime = 4;
    for (int pos = 0; pos < sizeof(dateTimeStr); pos++)
    {
        if (dateTimeStr[pos] == ' ')
        {
            count++;
        }
        if (count < gapsBetweenDateAndTime)
        {
            date[pos] = dateTimeStr[pos];
        }
        else
        {
            time[timeCount++] = dateTimeStr[pos];
        }
    }

    display(pageSelection, date, time);
    delay(10);
}

void displayStartingMenu()
{
    u8g2.clearBuffer();
    enableNavigation();
    for (int pos = 0; pos < numOptions; pos++)
    {
        if (currentSelection == pos)
        {
            u8g2.setCursor(0, positions[pos]);
            u8g2.print('>');
        }
        u8g2.setCursor(10, positions[pos]);
        u8g2.print(pageOneOptions[pos]);
    }

    if (!digitalRead(selectButton))
    {
        switch (currentSelection)
        {
        case 0:
        {
            pageSelection = 2;
            break;
        }
        case 1:
        {
            pageSelection = 3;
            break;
        }
        case 2:
        {
            displayStartingMenuOptionThree();
            break;
        }
        case 3:
        {
            displayStartingMenuOptionFour();
            break;
        }
        }
        while (!digitalRead(selectButton))
            ;
    }
    u8g2.sendBuffer();
}

void displayStartingMenuOptionOne(char date[], char time[])
{

    u8g2.clearBuffer();
    u8g2.setCursor(0, positions[0]);
    u8g2.print(date);
    u8g2.setCursor(0, positions[1]);
    u8g2.print(time + 1);
    u8g2.setCursor(0, positions[2]);
    u8g2.print("Press Select to go back");
    if (!digitalRead(selectButton))
    {
        pageSelection = 1;
    }
    while (!digitalRead(selectButton))
        ;
    u8g2.sendBuffer();
}

void displayStartingMenuOptionTwo()
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
        pageSelection = 1;
    }
    while (!digitalRead(selectButton))
        ;
    u8g2.sendBuffer();
}

void displayStartingMenuOptionThree()
{

    u8g2.clearBuffer();
    u8g2.setCursor(0, positions[2]);
    u8g2.print(3);
    u8g2.sendBuffer();
}

void displayStartingMenuOptionFour()
{

    u8g2.clearBuffer();
    u8g2.setCursor(0, positions[3]);
    u8g2.print(4);
    u8g2.sendBuffer();
}

void display(int pageSelection, char date[], char time[])
{
    switch (pageSelection)
    {
    case 1:
        displayStartingMenu();
        break;
    case 2:
        displayStartingMenuOptionOne(date, time);
        break;
    case 3:
        displayStartingMenuOptionTwo();
        break;
    }
}

void enableNavigation()
{
    if (!digitalRead(upButton))
    {
        currentSelection--;
        if (currentSelection < 0)
            currentSelection += numOptions;
        while (!digitalRead(upButton))
            ;
    }
    if (!digitalRead(downButton))
    {
        currentSelection++;
        currentSelection %= numOptions;
        while (!digitalRead(downButton))
            ;
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