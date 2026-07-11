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
    DateAndTime dateAndTime = getDateAndTime();

    char *day = dateAndTime.day;
    char *date = dateAndTime.date;
    char *time = dateAndTime.time;

    // Day and date
    u8g2.setFont(u8g2_font_t0_11b_tf);
    int dayWidth = u8g2.getStrWidth(day);
    int dateWidth = u8g2.getStrWidth(date);
    u8g2.setCursor((128 - dayWidth - dateWidth) / 2, positions[0]);
    u8g2.print(day);
    u8g2.setFont(u8g2_font_profont12_tf);
    u8g2.setCursor((128 + 2 * dayWidth - dateWidth) / 2, positions[0]);
    u8g2.print(date);

    // Time
    u8g2.setFont(u8g2_font_fub17_tn);
    int timeWidth = u8g2.getStrWidth(time);
    u8g2.setCursor((128 - timeWidth) / 2, 40);
    u8g2.print(time);

    // Divider line
    u8g2.drawLine(0, positions[2], 127, positions[2]);

    // Indicator showing selected page in app
    int x = 50;
    for (int i = 0; i < 4; i++)
    {
        if (i == selectedFeature)
        {
            u8g2.drawDisc(x, 56, 2);
        }
        else
        {
            u8g2.drawCircle(x, 56, 2);
        }
        x += 9;
    }
    if (!digitalRead(selectButton) && !isSelectDebouncing)
    {
        displayPage = 1;
    }
    selectDelay();
    u8g2.sendBuffer();
}
