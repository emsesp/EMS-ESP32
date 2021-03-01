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

#ifndef _PButton_H_
#define _PButton_H_

#include <Arduino.h>

class PButton {
  public:
    PButton();
    ~PButton() = default;

    typedef void (*buttonEventHandler)(PButton &);

    void onClick(uint16_t t, buttonEventHandler handler);
    void onDblClick(uint16_t, buttonEventHandler handler);
    void onLongPress(uint16_t t, buttonEventHandler handler);
    void onVLongPress(uint16_t, buttonEventHandler handler);
    bool init(uint8_t pin, bool pullMode);
    bool check(void);

  private:
    uint16_t Debounce_;        // Debounce period to prevent flickering when pressing or releasing the button (in ms)
    uint16_t DblClickDelay_;   // Max period between clicks for a double click event (in ms)
    uint16_t LongPressDelay_;  // Hold period for a long press event (in ms)
    uint16_t VLongPressDelay_; // Hold period for a very long press event (in ms)

    uint8_t pin_;
    bool    pullMode_;
    bool    enabled_;

    bool state_;            // Value read from button
    bool lastState_;        // Last value of button state
    bool dblClickWaiting_;  // whether we're waiting for a double click (down)
    bool dblClickOnNextUp_; // whether to register a double click on next release, or whether to wait and click
    bool singleClickOK_;    // whether it's OK to do a single click

    uint32_t downTime_; // time the button was pressed down
    uint32_t upTime_;   // time the button was released

    bool ignoreUP_;           // whether to ignore the button release because the click+hold was triggered
    bool waitForUP_;          // when held, whether to wait for the up event
    bool longPressHappened_;  // whether or not the hold event happened already
    bool vLongPressHappened_; // whether or not the long hold event happened already

    bool buttonBusy_; // false if idle

    buttonEventHandler cb_onClick, cb_onDblClick, cb_onLongPress, cb_onVLongPress;
};

#endif
