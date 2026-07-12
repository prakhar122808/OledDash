#include "header.h"

// U8g2 object for SH1106 128x64 OLED display using hardware I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// WiFi credentials and NTP server configuration
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *ntpServer = "in.pool.ntp.org";
const long gmtOffset = 19800; // +5:30(IST)
const long daylightOffset = 0;

// ----------------------Menu options and positions----------------------
int positions[] = {16, 32, 48, 63};
const char *mainMenu[] = {"Date and Time", "Weather"};
const int numFeatures = sizeof(mainMenu) / sizeof(mainMenu[0]);

// ------------Variables to track current selection and page------------
int selection = 1;
int displayPage = 1;
int selectedFeaturePage = 1;
int totalFeaturePages = 1;

// ---------------------Variables related to weather---------------------
Weather cachedWeather = {-1, -1, -1, " ", " "};
unsigned long lastFetchTime = 0;
const unsigned long fetchInterval = 60000; // 1 minute

// ---------------------------NonBlockingDelays---------------------------
// Select button
NonBlockingDelay selectDebounce(250);
bool lastSelectState = HIGH;
bool isSelectDebouncing = false;

// Next button
NonBlockingDelay nextDebounce(250);
bool lastNextState = HIGH;
bool isNextDebouncing = false;

// Prev button
NonBlockingDelay prevDebounce(250);
bool lastPrevState = HIGH;
bool isPrevDebouncing = false;