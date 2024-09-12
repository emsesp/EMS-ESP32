/*
 * Paul's Button Library
 * 
 * Handles recognition of button presses, short, long, double-click and special sequences
 * Used to send specific commands to the ESP8
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PButton.h"

// Constructor
PButton::PButton() {
    // Initialization of default properties
    Debounce_        = 40;   // Debounce period to prevent flickering when pressing or releasing the button (in ms)
    DblClickDelay_   = 250;  // Max period between clicks for a double click event (in ms)
    LongPressDelay_  = 750;  // Hold period for a long press event (in ms)
    VLongPressDelay_ = 3000; // Hold period for a very long press event (in ms)

    cb_onClick      = nullptr;
    cb_onDblClick   = nullptr;
    cb_onLongPress  = nullptr;
    cb_onVLongPress = nullptr;

    // Initialization of variables
    state_              = true;  // Value read from button
    lastState_          = true;  // Last value of button state
    dblClickWaiting_    = false; // whether we're waiting for a double click (down)
    dblClickOnNextUp_   = false; // whether to register a double click on next release, or whether to wait and click
    singleClickOK_      = true;  // whether it's OK to do a single click (default is true)
    downTime_           = -1;    // time the button was pressed down
    upTime_             = -1;    // time the button was released
    ignoreUP_           = true;  // whether to ignore the button release because the click+hold was triggered, changed from false for ESP32
    waitForUP_          = false; // when held, whether to wait for the up event
    longPressHappened_  = false; // whether or not the hold event happened already
    vLongPressHappened_ = false; // whether or not the long hold event happened already
    buttonBusy_         = false; // idle
    pin_                = 255;   // undefined
}

bool PButton::init(uint8_t pin, bool pullMode) {
    pin_      = pin;
    pullMode_ = pullMode; // 1=HIGH (pullup) 0=LOW (pulldown)

#if defined(ESP32)
    pinMode(pin_, pullMode ? INPUT_PULLUP : INPUT_PULLDOWN);
#else // esp8266 and standalone
    pinMode(pin_, pullMode ? INPUT_PULLUP : INPUT);
#endif
    enabled_ = (digitalRead(pin_) == pullMode); // see if a button is connected

    return enabled_;
}

void PButton::onClick(uint16_t t, buttonEventHandler handler) {
    Debounce_  = t; // Debounce period to prevent flickering when pressing or releasing the button (in ms)
    cb_onClick = handler;
}

void PButton::onDblClick(uint16_t t, buttonEventHandler handler) {
    DblClickDelay_ = t; // Max period between clicks for a double click event (in ms)
    cb_onDblClick  = handler;
}

void PButton::onLongPress(uint16_t t, buttonEventHandler handler) {
    LongPressDelay_ = t; // Hold period for a long press event (in ms)
    cb_onLongPress  = handler;
}

void PButton::onVLongPress(uint16_t t, buttonEventHandler handler) {
    VLongPressDelay_ = t; // Hold period for a very long press event (in ms)
    cb_onVLongPress  = handler;
}

bool PButton::check(void) {
    if (!enabled_ || pin_ == 255) {
        return false;
    }

    int  resultEvent = 0;
    long millisRes   = millis();

    // make sure the pin is still input
#if defined(ESP32)
    pinMode(pin_, pullMode_ ? INPUT_PULLUP : INPUT_PULLDOWN);
#else // esp8266 and standalone
    pinMode(pin_, pullMode_ ? INPUT_PULLUP : INPUT);
#endif
    state_ = digitalRead(pin_) == pullMode_;

    // Button pressed down
    if (state_ != pullMode_ && lastState_ == pullMode_ && (millisRes - upTime_) > Debounce_) {
        downTime_           = millisRes;
        ignoreUP_           = false;
        waitForUP_          = false;
        singleClickOK_      = true;
        longPressHappened_  = false;
        vLongPressHappened_ = false;
        if ((millisRes - upTime_) < DblClickDelay_ && dblClickOnNextUp_ == false && dblClickWaiting_ == true) {
            dblClickOnNextUp_ = true;
        } else {
            dblClickOnNextUp_ = false;
        }
        dblClickWaiting_ = false;

        buttonBusy_ = true; // something is happening so we'll wait and see what action it is
    }

    // Button released
    else if (state_ == pullMode_ && lastState_ != pullMode_ && (millisRes - downTime_) > Debounce_) {
        if (ignoreUP_ == false) {
            upTime_ = millisRes;
            if (dblClickOnNextUp_ == false) {
                dblClickWaiting_ = true;
            } else {
                resultEvent       = 2;
                dblClickOnNextUp_ = false;
                dblClickWaiting_  = false;
                singleClickOK_    = false;
            }
        }
    }

    // Test for normal click event: DblClickDelay expired
    if (state_ == pullMode_ && (millisRes - upTime_) >= DblClickDelay_ && dblClickWaiting_ == true && dblClickOnNextUp_ == false && singleClickOK_ == true
        && resultEvent != 2) {
        resultEvent      = 1;
        dblClickWaiting_ = false;
    }

    // added code: raise OnLongPress event when the button is released
    if (state_ == pullMode_ && longPressHappened_ && !vLongPressHappened_) {
        resultEvent        = 3;
        longPressHappened_ = false;
    }

    // added code: raise OnVLongPress event when the button is released, only for pin 0
    if (state_ == pullMode_ && vLongPressHappened_) {
        resultEvent         = 4;
        vLongPressHappened_ = false;
        longPressHappened_  = false;
    }

    // Test for hold
    if (state_ != pullMode_ && (millisRes - downTime_) >= LongPressDelay_) {
        // Trigger "normal" hold
        if (longPressHappened_ == false) {
            // resultEvent = 3;
            waitForUP_        = true;
            ignoreUP_         = true;
            dblClickOnNextUp_ = false;
            dblClickWaiting_  = false;
            // _downTime = millis();
            longPressHappened_ = true;
        }
        // Trigger "long" hold
        if ((millisRes - downTime_) >= VLongPressDelay_) {
            if (vLongPressHappened_ == false) {
                // raise OnVLongPress, only for pin 0 wait for release
                if (pin_) {
                    resultEvent = 4;
                }
                vLongPressHappened_ = true;
                // longPressHappened_ = false;
            }
        }
    }

    lastState_ = state_;

    if (resultEvent == 1 && cb_onClick) {
        cb_onClick(*this);
    } else if (resultEvent == 2 && cb_onDblClick) {
        cb_onDblClick(*this);
    } else if (resultEvent == 3 && cb_onLongPress) {
        cb_onLongPress(*this);
    } else if (resultEvent == 4 && cb_onVLongPress) {
        cb_onVLongPress(*this);
    }

    // if any action has been preformed we can stop waiting, and become idle
    if (resultEvent >= 1 && resultEvent <= 4) {
        buttonBusy_ = false;
    }

    return buttonBusy_;
}
