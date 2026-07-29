// Button.h
#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button{
private:

    static const unsigned long repeatStartDelay = 200;
    static const unsigned long repeatInterval   = 80;
    static const unsigned long longPressDuration = 1500;
    static const unsigned long debounceDelay    = 120;

    uint8_t pinNumber;

    bool lastButtonState;

    unsigned long pressStartTime;
    unsigned long lastRepeatTime;
    unsigned long lastDebounceTime;

    bool longPressTriggered;

public:

    enum class Event {
        NONE,
        SHORT,
        REPEAT,
        LONG
    };

    Button(uint8_t pinNumber){

        this->pinNumber = pinNumber;

        lastButtonState = HIGH;

        pressStartTime = 0;
        lastRepeatTime = 0;
        lastDebounceTime = 0;

        longPressTriggered = false;
    }

    Event read(){
        bool currentButtonState = digitalRead(pinNumber);
        unsigned long currentTime = millis();

        // Debounce
        if (currentTime - lastDebounceTime < debounceDelay &&
            currentButtonState != lastButtonState)
        {
            return Event::NONE;
        }

        // Button Pressed
        if (lastButtonState == HIGH &&
            currentButtonState == LOW)
        {
            pressStartTime = currentTime;
            lastRepeatTime = currentTime;
            lastDebounceTime = currentTime;

            longPressTriggered = false;
        }

        // Button Holding
        if (currentButtonState == LOW)
        {
            if (!longPressTriggered &&
                currentTime - pressStartTime >= longPressDuration)
            {
                longPressTriggered = true;
                return Event::LONG;
            }

            // Repeat (KEEP RUNNING EVEN AFTER LONG)
            if (currentTime - pressStartTime >= repeatStartDelay)
            {
                if (currentTime - lastRepeatTime >= repeatInterval)
                {
                    lastRepeatTime = currentTime;
                    return Event::REPEAT;
                }
            }
        }

        // Button Released (SHORT PRESS)
        if (lastButtonState == LOW &&
            currentButtonState == HIGH)
        {
            lastDebounceTime = currentTime;

            if (!longPressTriggered &&
                currentTime - pressStartTime < longPressDuration)
            {
                lastButtonState = currentButtonState;
                return Event::SHORT;
            }
        }

        lastButtonState = currentButtonState;

        return Event::NONE;
    }
};

#endif