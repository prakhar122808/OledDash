#include <Arduino.h>
#include <ArduinoJson.h>
#include <cstring>
#include <HTTPClient.h>
#include <time.h>
#include <U8g2lib.h>
#include <WiFi.h>

#include "credentials.h"
#include "nonBlockingDelay.h"

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
// #define downButton 17
#define selectButton 18

// ----------------------Menu options and positions----------------------
int positions[] = {16, 32, 48, 64};
const char *mainMenu[] = {"Date and Time", "Weather"};
const int numFeatures = sizeof(mainMenu) / sizeof(mainMenu[0]);

// ------------Variables to track current selection and page------------
int selection = 0;
int displayPage = 1;

// ------------------Variables related to date and time------------------
struct DateAndTime
{
    char day[4];   // eg. MON
    char date[11]; // eg. 01/11/2025
    char time[9];  // eg. 23:09:38
};

// ---------------------Variables related to weather---------------------
struct Temperatures
{
    float actualTemp;
    float feelsLikeTemp;
};

Temperatures cachedTemps = {-1, -1};
unsigned long lastFetchTime = 0;
const unsigned long fetchInterval = 60000; // 1 minute

// -------------------------Function declarations-------------------------
// Displays
void displayStartingMenu();
void displayTime();
void displayWeather();
void display(int displayPage);
// Custom delays
void selectDelay();
void upDelay();
// Helper functions
Temperatures getTemps();
DateAndTime getDateAndTime();

// ---------------------------NonBlockingDelays---------------------------
// Select button
NonBlockingDelay selectDebounce(250);
bool lastSelectState = HIGH;
bool isSelectDebouncing = false;
// Up button
NonBlockingDelay upDebounce(500);
bool lastUpState = HIGH;
bool isUpDebouncing = false;

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

    pinMode(upButton, INPUT_PULLUP);
    // pinMode(downButton, INPUT_PULLUP);
    pinMode(selectButton, INPUT_PULLUP);
}

void loop()
{

    display(displayPage);
    delay(100);
}

void displayStartingMenu()
{
    u8g2.clearBuffer();

    if (!digitalRead(upButton) && !isUpDebouncing)
    {
        selection = (selection + 1) % 2;
    }
    // 2x2 grid placement positions
    int colX[2] = {20, 84};
    int rowY[2] = {25, 55};

    // Application labels
    const char *appNames[] = {"Clock", "Weather"};

    const int numApps = sizeof(appNames) / sizeof(appNames[0]);
    for (int i = 0; i < numApps; i++)
    {
        int col = i % 2;
        int row = i / 2;
        int x = colX[col];
        int y = rowY[row];

        // Draw frame around selected app
        if (i == selection)
        {
            u8g2.drawRFrame(x - 14, y - 20, 52, 30, 4);
        }

        // --- Icons ---
        const int appClock = 69;
        const int weatherSunBehindClouds = 65;
        if (i == 0)
        {
            u8g2.setFont(u8g2_font_open_iconic_app_2x_t);
            u8g2.drawGlyph(x + 4, y - 4, appClock);
        }
        else if (i == 1)
        {
            u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
            u8g2.drawGlyph(x + 4, y - 4, weatherSunBehindClouds);
        }

        // ---Text styling---
        u8g2.setFont(u8g2_font_6x10_tf);

        int textWidth = u8g2.getStrWidth(appNames[i]);
        int textX = x + 12 - (textWidth / 2);

        u8g2.drawStr(textX, y + 8, appNames[i]);
    }

    if (!digitalRead(selectButton) && !isSelectDebouncing)
    {
        switch (selection)
        {
        case 0:
        {
            displayPage = 2;
            u8g2.setFont(u8g2_font_5x7_t_cyrillic);
            u8g2.setCursor(64, positions[3] - 2);
            u8g2.print("Loading...");
            break;
        }
        case 1:
        {
            displayPage = 3;
            u8g2.setFont(u8g2_font_5x7_t_cyrillic);
            u8g2.setCursor(64, positions[3] - 2);
            u8g2.print("Loading...");
            break;
        }
        }
    }
    upDelay();
    selectDelay();
    u8g2.sendBuffer();
}

void displayTime()
{
    // Plan to add option to use external hardware to get time
    // and defaulting to NTP when external hardware isn't present

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_t0_11b_tf);
    DateAndTime dateAndTime = getDateAndTime();

    char *day = dateAndTime.day;
    char *date = dateAndTime.date;
    char *time = dateAndTime.time;

    u8g2.setCursor(0, positions[0]);
    u8g2.print(day);
    u8g2.setCursor(0, positions[1]);
    u8g2.print(date);
    u8g2.setCursor(0, positions[2]);
    u8g2.print(time);
    u8g2.setCursor(0, positions[3]);
    u8g2.print("Select to go back");
    if (!digitalRead(selectButton) && !isSelectDebouncing)
    {
        displayPage = 1;
    }
    selectDelay();
    u8g2.sendBuffer();
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

void display(int displayPage)
{
    switch (displayPage)
    {
    case 1:
        displayStartingMenu();
        break;
    case 2:
        displayTime();
        break;
    case 3:
        displayWeather();
        break;
    }
}

void selectDelay()
{
    bool currentSelectState = digitalRead(selectButton);

    if (isSelectDebouncing && selectDebounce.isElapsed())
    {
        isSelectDebouncing = false;
    }

    if (currentSelectState == LOW && lastSelectState == HIGH && !isSelectDebouncing)
    {
        selectDebounce.start();
        isSelectDebouncing = true;
    }
    lastSelectState = currentSelectState;
}

void upDelay()
{
    bool currentUpState = digitalRead(upButton);

    if (isUpDebouncing && upDebounce.isElapsed())
    {
        isUpDebouncing = false;
    }

    if (currentUpState == LOW && lastUpState == HIGH && !isUpDebouncing)
    {
        upDebounce.start();
        isUpDebouncing = true;
    }
    lastUpState = currentUpState;
}

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

DateAndTime getDateAndTime()
{

    // Get the current date and time
    struct tm timeInfo;
    DateAndTime dateAndTime = {"", "", ""};

    if (!getLocalTime(&timeInfo))
    {
        Serial.println("Failed to obtain time.");
        return dateAndTime;
    }
    // Initialise to zero
    char day[4] = {0};
    char date[11] = {0};
    char time[9] = {0};

    strftime(day, sizeof(day), "%A", &timeInfo);
    // strftime(date, 2, "%b", &timeInfo);
    // converToMonthNumber(date);
    strftime(date, sizeof(date), "%d/%m/%Y", &timeInfo);
    strftime(time, sizeof(time), "%H:%M:%S", &timeInfo);

    // Return value
    std::strcpy(dateAndTime.day, day);
    std::strcpy(dateAndTime.date, date);
    std::strcpy(dateAndTime.time, time);
    return dateAndTime;
}
