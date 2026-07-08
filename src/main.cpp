#include <Arduino.h>
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
const long gmtOffset = 19800; // +5:30
const long daylightOffset = 0;

// Button pin definitions
#define upButton 16
#define downButton 17
#define selectButton 18
#define numOptions 4

// Menu options and positions
int positions[] = {16, 32, 48, 64};
const char *pageOneOptions[] = {"Date and Time", "Option 2", "Option 3", "Option 4"};

// Variables to track current selection and page
int currentSelection = 0;
int pageSelection = 1;

// Function declarations
void displayStartingMenu();
void displayStartingMenuOptionOne(char date[], char time[]);
void displayStartingMenuOptionTwo();
void displayStartingMenuOptionThree();
void displayStartingMenuOptionFour();
void enableNavigation();

void display(int pageSelection, char date[], char time[]);

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
            displayStartingMenuOptionTwo();
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
    u8g2.setCursor(0, positions[1]);
    u8g2.print(2);
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