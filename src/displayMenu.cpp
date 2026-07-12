#include "header.h"

void displayStartingMenu()
{
    u8g2.clearBuffer();

    // Application labels
    const char *appNames[] = {"Clock", "Weather"};

    // Number of apps
    const int numApps = sizeof(appNames) / sizeof(appNames[0]);

    // 2x2 grid placement positions
    int colX[2] = {20, 84};
    int rowY[2] = {25, 55};

    for (int i = 0; i < numApps; i++)
    {
        int col = i % 2;
        int row = i / 2;
        int x = colX[col];
        int y = rowY[row];

        int j = i + 1;
        // Draw frame around selected app
        if (j == selection)
        {
            u8g2.drawRFrame(x - 14, y - 20, 52, 30, 4);
        }

        // --- Icons ---
        const int appClock = 69;
        const int weatherSunBehindClouds = 65;
        if (j == 1)
        {
            u8g2.setFont(u8g2_font_open_iconic_app_2x_t);
            u8g2.drawGlyph(x + 4, y - 4, appClock);
        }
        else if (j == 2)
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

    if (!digitalRead(nextButton) && !isNextDebouncing)
    {
        if (selection != numApps)
            selection++;
        else
            selection = 1;
    }
    if (!digitalRead(prevButton) && !isPrevDebouncing)
    {
        if (selection != 1)
            selection--;
        else
            selection = numApps;
    }
    if (!digitalRead(selectButton) && !isSelectDebouncing)
    {
        switch (selection)
        {
        case 1:
        {
            displayPage = 2;
            u8g2.setFont(u8g2_font_5x7_t_cyrillic);
            u8g2.setCursor(64, positions[3] - 2);
            u8g2.print("Loading...");
            break;
        }
        case 2:
        {
            displayPage = 3;
            u8g2.setFont(u8g2_font_5x7_t_cyrillic);
            u8g2.setCursor(64, positions[3] - 2);
            u8g2.print("Loading...");
            break;
        }
        }
    }

    nextDelay();
    prevDelay();
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
