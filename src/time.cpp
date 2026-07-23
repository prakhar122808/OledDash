#include <cstring>

#include "header.h"

DateAndTime getDateAndTime();
void displayTimeFeature(int selectedFeaturePage);
void currentTime();
void stopwatch();

unsigned long stopwatchTime;
unsigned long elapsedTime = 0;
short timeCounter;
short seconds;
short minutes;
short hours;
bool isStopwatchRunning = false;

void displayTime()
{
    const char *features[] = {"currentTime", "stopwatch"};
    totalFeaturePages = sizeof(features) / sizeof(features[0]);
    // Plan to add option to use external hardware to get time
    // and defaulting to NTP when external hardware isn't present
    displayTimeFeature(selectedFeaturePage);
    // Indicator showing selected page in app
    int x = 68 - (9 * totalFeaturePages / 2);
    for (int i = 1; i <= totalFeaturePages; i++)
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
    if (!digitalRead(nextButton) && !isNextDebouncing && selectedFeaturePage != totalFeaturePages)
    {
        selectedFeaturePage++;
    }
    // if (!digitalRead(prevButton) && !isPrevDebouncing && selectedFeaturePage != 1)
    // {
    //     selectedFeaturePage--;
    // }
    if (!digitalRead(selectButton) && !isSelectDebouncing)
    {
        displayPage = 1;
    }

    nextDelay();
    // prevDelay();
    selectDelay();
    u8g2.sendBuffer();
}

// ---------------------------------Page 1----------------------------------
void currentTime()
{
    u8g2.clearBuffer();
    DateAndTime dateAndTime = getDateAndTime();

    char *day = dateAndTime.day;
    char *date = dateAndTime.date;
    char *time = dateAndTime.time;

    // Day and date
    u8g2.setFont(u8g2_font_t0_11b_tf);
    int dayWidth = u8g2.getStrWidth(day);

    u8g2.setFont(u8g2_font_profont12_tf);
    int dateWidth = u8g2.getStrWidth(date);

    int spaceWidth = 4;
    int totalDateWidth = dayWidth + spaceWidth + dateWidth;
    int startPos = (128 - totalDateWidth) / 2;

    u8g2.setFont(u8g2_font_t0_11b_tf);
    u8g2.setCursor(startPos, positions[0]);
    u8g2.print(day);

    u8g2.setFont(u8g2_font_profont12_tf);
    u8g2.setCursor(startPos + dayWidth + spaceWidth, positions[0]);
    u8g2.print(date);

    // Time
    u8g2.setFont(u8g2_font_fub17_tn);
    int timeWidth = u8g2.getStrWidth(time);
    u8g2.setCursor((128 - timeWidth) / 2, 40);
    u8g2.print(time);
}

// ---------------------------------Page 2----------------------------------
void stopwatch()
{
    u8g2.clearBuffer();
    if (!digitalRead(prevButton) && !isPrevDebouncing)
    {
        if (!isStopwatchRunning)
        {
            startTime = millis();
            isStopwatchRunning = true;
        }
        else
        {
            elapsedTime += millis() - startTime;
            isStopwatchRunning = false;
        }
    }
    if (isStopwatchRunning)
    {
        stopwatchTime = elapsedTime + (millis() - startTime);
    }
    else
    {
        stopwatchTime = elapsedTime;
    }
    timeCounter = stopwatchTime / 1000;
    seconds = timeCounter % 60;
    minutes = (timeCounter / 60) % 60;
    hours = timeCounter / 3600;

    char timeBuffer[10];
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", hours, minutes, seconds);
    u8g2.setFont(u8g2_font_fub17_tn);
    u8g2.setCursor(16, 32);
    prevDelay();
    u8g2.print(timeBuffer);
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
    std::snprintf(dateAndTime.day, sizeof(dateAndTime.day), "%s", day);
    std::snprintf(dateAndTime.date, sizeof(dateAndTime.date), "%s", date);
    std::snprintf(dateAndTime.time, sizeof(dateAndTime.time), "%s", time);
    return dateAndTime;
}

void displayTimeFeature(int selectedFeaturePage)
{
    switch (selectedFeaturePage)
    {
    case 1:
        currentTime();
        break;
    case 2:
        stopwatch();
        break;
    }
}
