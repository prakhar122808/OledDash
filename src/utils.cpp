#include "header.h"

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
