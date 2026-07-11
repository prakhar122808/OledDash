#include <cstring>

#include "header.h"

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
