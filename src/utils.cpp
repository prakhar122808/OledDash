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

void nextDelay()
{
    bool currentNextState = digitalRead(nextButton);

    if (isNextDebouncing && nextDebounce.isElapsed())
    {
        isNextDebouncing = false;
    }

    if (currentNextState == LOW && lastNextState == HIGH && !isNextDebouncing)
    {
        nextDebounce.start();
        isNextDebouncing = true;
    }
    lastNextState = currentNextState;
}

void prevDelay()
{
    bool currentPrevState = digitalRead(prevButton);

    if (isPrevDebouncing && prevDebounce.isElapsed())
    {
        isPrevDebouncing = false;
    }

    if (currentPrevState == LOW && lastPrevState == HIGH && !isPrevDebouncing)
    {
        prevDebounce.start();
        isPrevDebouncing = true;
    }
    lastPrevState = currentPrevState;
}
