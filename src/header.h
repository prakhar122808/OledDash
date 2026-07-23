#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "credentials.h"
#include "nonBlockingDelay.h"

// Button pin definitions
#define nextButton 16
#define prevButton 17
#define selectButton 18

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

extern const char *ssid;
extern const char *password;

extern const char *ntpServer;
extern const long gmtOffset;
extern const long daylightOffset;

extern int positions[];
extern const char *mainMenu[];
extern const int numFeatures;

extern int selection;
extern int displayPage;
extern int selectedFeaturePage;
extern int totalFeaturePages;

extern unsigned long lastPublish;
extern const unsigned long PUBLISH_INTERVAL;

// ------------------Variables related to date and time------------------
struct DateAndTime
{
    char day[4];   // eg. MON
    char date[11]; // eg. 01/11/2025
    char time[9];  // eg. 23:09:38
};
extern unsigned long startTime;
// ---------------------Variables related to weather---------------------
struct Weather
{
    float actualTemp;
    float feelsLikeTemp;
    float humidity;
    char weather[17];    // eg. Scattered clouds
    char weatherIcon[4]; // eg. 01d
};

extern Weather cachedWeather;
extern unsigned long lastFetchTime;
extern const unsigned long fetchInterval;

// ---------------------------NonBlockingDelays---------------------------

extern NonBlockingDelay selectDebounce;
extern bool lastSelectState;
extern bool isSelectDebouncing;

extern NonBlockingDelay nextDebounce;
extern bool lastNextState;
extern bool isNextDebouncing;

extern NonBlockingDelay prevDebounce;
extern bool lastPrevState;
extern bool isPrevDebouncing;

// Configure secure client
extern WiFiClient net;

// MQTT
extern PubSubClient client;

// -------------------------Function declarations-------------------------
// Displays
void display(int displayPage);
void displayStartingMenu();

// Display related to clock
void displayTime();

// Display related to weather
void displayWeather();

float getActualTemp();
float getFeelsLikeTemp();

// Custom delays
void selectDelay();
void nextDelay();
void prevDelay();

// Helper functions
Weather getWeather();
DateAndTime getDateAndTime();

// WiFi
void startWifi();

// MQTT
void connectAWS();
void publishData(float actualTemp, float feelsLikeTemp);