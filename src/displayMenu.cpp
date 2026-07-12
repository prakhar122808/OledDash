#include "header.h"

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

void displayUp()
{
    u8g2.setFont(u8g2_font_cu12_t_symbols);
    u8g2.drawGlyph(120, 12, 0x2191); // Up arrow
}
void displayDown()
{
    u8g2.setFont(u8g2_font_cu12_t_symbols);
    u8g2.drawGlyph(120, 64, 0x2193); // Down arrow
}