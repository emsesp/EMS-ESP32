/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
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

#include "led.h"
#include "emsesp.h"

#ifndef EMSESP_STANDALONE
#include <esp32-hal-rmt.h>
#endif

namespace emsesp {

namespace rgb_led {

#ifndef EMSESP_STANDALONE

namespace {

// durations in 100 ns ticks
constexpr uint16_t T0H = 3; // 300 ns
constexpr uint16_t T0L = 9; // 900 ns
constexpr uint16_t T1H = 8; // 800 ns
constexpr uint16_t T1L = 6; // 600 ns

// rmt_data_t.val layout: duration0:15, level0:1, duration1:15, level1:1 (high then low)
constexpr uint32_t SYMBOL_0 = T0H | (1UL << 15) | (static_cast<uint32_t>(T0L) << 16);
constexpr uint32_t SYMBOL_1 = T1H | (1UL << 15) | (static_cast<uint32_t>(T1L) << 16);

constexpr uint32_t RMT_FREQ_HZ = 10000000; // 10 MHz
constexpr uint32_t RESET_US    = 300;
constexpr uint8_t  NO_PIN      = 0xFF;
constexpr uint8_t  MAX_PINS    = 2; // status LED + an RGB analog sensor; the ESP32-C3 only has 2 RMT TX channels

struct Channel {
    uint8_t  pin        = NO_PIN;
    uint32_t last_tx_us = 0;
};

Channel channels[MAX_PINS];

Channel * init(uint8_t pin) {
    Channel * free_slot = nullptr;
    for (auto & ch : channels) {
        if (ch.pin == pin) {
            return &ch;
        }
        if (!free_slot && ch.pin == NO_PIN) {
            free_slot = &ch;
        }
    }

    if (!free_slot) {
        // all slots in use: release the least recently written one
        const uint32_t now = micros();
        free_slot          = &channels[0];
        for (auto & ch : channels) {
            if (now - ch.last_tx_us > now - free_slot->last_tx_us) {
                free_slot = &ch;
            }
        }
        end(free_slot->pin);
    }

    if (!rmtInit(pin, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_1, RMT_FREQ_HZ)) {
        return nullptr;
    }
    free_slot->pin        = pin;
    free_slot->last_tx_us = micros() - RESET_US; // no wait on first write
    return free_slot;
}

} // namespace

bool write(uint8_t pin, uint8_t red, uint8_t green, uint8_t blue, Order order) {
    Channel * ch = init(pin);
    if (!ch) {
        return false;
    }

    // enforce the latch/reset gap since the previous frame (V6 needs > 280 us)
    const uint32_t elapsed = micros() - ch->last_tx_us;
    if (elapsed < RESET_US) {
        delayMicroseconds(RESET_US - elapsed);
    }

    const uint8_t bytes[3] = {order == Order::GRB ? green : red, order == Order::GRB ? red : green, blue};

    rmt_data_t frame[24];
    for (uint8_t i = 0; i < 24; i++) {
        frame[i].val = (bytes[i >> 3] & (0x80 >> (i & 7))) ? SYMBOL_1 : SYMBOL_0;
    }

    const bool ok  = rmtWrite(pin, frame, 24, RMT_WAIT_FOR_EVER);
    ch->last_tx_us = micros();
    return ok;
}

void end(uint8_t pin) {
    for (auto & ch : channels) {
        if (ch.pin == pin && pin != NO_PIN) {
            rmtDeinit(pin);
            ch.pin = NO_PIN;
        }
    }
}

#else

bool write(uint8_t, uint8_t, uint8_t, uint8_t, Order) {
    return true;
}

void end(uint8_t) {
}

#endif

} // namespace rgb_led

uuid::log::Logger LED::logger_{F_(led), uuid::log::Facility::KERN};

// initialise the LED, fetching the settings from the WebSettingsService
// set the LED to on or off when in normal operating mode
void LED::init() {
    // settings may have changed: turn off and release the previous pin
    if (led_gpio_) {
        set_led(Color::OFF);
        if (led_type_) {
            rgb_led::end(led_gpio_);
        }
    }

    // copy the application settings
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        led_gpio_ = settings.led_gpio;
        led_type_ = settings.led_type;
        hide_led_ = settings.hide_led;
    });
    last_color_ = COLOR_UNSET;

    if (!led_gpio_) { // 0 means disabled
        LOG_INFO("LED disabled");
        return;
    }

    // for safety
    if (!led_type_) {
        pinMode(led_gpio_, OUTPUT);
    }

    reset_led(); // start with LED in default state, depending on if it's hidden or not
}

// handle LED routine
// called from the System::loop()
// returns true if the LED flash is active, i.e its a lock down state
bool LED::loop(uint8_t healthcheck, bool button_busy) {
    // if LED flashing is active it means its about to perform a factory reset, so don't do anything else and keep it flashing
    if (led_fast_flash_timer_) {
        led_fast_flash();
        return true;
    }

    // the user-requested LED blink always has preference
    if (!is_user_led_blink_) {
        // check for button press.
        // while the button is held show the LED (yellow on RGB LED, on/off on standard LED),
        // and on release go back to the default state and re-check the health on the next cycle
        if (last_button_busy_ != button_busy) {
            last_button_busy_ = button_busy;
            if (button_busy) {
                set_led(Color::YELLOW); // Yellow
            } else {
                reset_led();
            }
            return false;
        }

        // check the system health.
        // Set the sequence accordingly, only if the healthcheck is not 0 and has changed
        if (healthcheck != previous_healthcheck_) {
            // if the healthcheck is 0, i.e. system is healthy, reset the LED
            if (healthcheck == 0) {
                reset_led();
            } else {
                //  1 flash (blue) is the EMS bus is not connected
                //  2 flashes (red, red) if the network (wifi or ethernet) is not connected
                //  3 flashes (red, red, blue) is both the bus and the network are not connected
                const bool no_network = (healthcheck & System::HEALTHCHECK_NO_NETWORK) == System::HEALTHCHECK_NO_NETWORK;
                const bool no_bus     = (healthcheck & System::HEALTHCHECK_NO_BUS) == System::HEALTHCHECK_NO_BUS;

                color_steps_[0] = no_network ? Color::RED : (no_bus ? Color::BLUE : Color::OFF);
                color_steps_[1] = no_network ? Color::RED : Color::OFF;
                color_steps_[2] = (no_network && no_bus) ? Color::BLUE : Color::OFF;
            }

            previous_healthcheck_ = healthcheck; // must be set after reset_led(), which invalidates it
        }

        // there's nothing to flash, so leave the LED in the state reset_led() gave it.
        // Running the sequence here would keep writing Color::OFF and a healthy system would never show green.
        if (healthcheck == 0 || button_busy) {
            return false;
        }
    }

    // show the LED status based on the healthcheck and button busy status
    sequence_led();

    return false;
}

// turn the LED back it's default state depending on if it's hidden or not
void LED::reset_led() {
    is_user_led_blink_ = false;
    set_led(hide_led_ ? Color::OFF : Color::GREEN); // Green
    color_steps_[0] = color_steps_[1] = color_steps_[2] = Color::OFF;

    // invalidate the last health state so the sequence is rebuilt on the next check,
    // needed because the color steps above have been cleared
    previous_healthcheck_ = System::HEALTHCHECK_RESET;

    // rewind the sequence so it starts from the top if the health degrades again
    led_long_timer_  = 1; // 1 will kick it off immediately
    led_short_timer_ = 0;
    led_flash_step_  = 0;
}

// LED flash every few ms and then perform a factory reset
void LED::led_fast_flash() {
    uint32_t current_time = uuid::get_uptime();

    if (current_time - last_toggle_time_ >= LED_FLASH_INTERVAL_MS) {
        led_flash_state_  = !led_flash_state_;
        last_toggle_time_ = current_time;
        set_led(led_flash_state_ ? Color::YELLOW : Color::OFF); // Yellow
    }

    // after duration, turn off the LED, and call the format command
    if (current_time - led_flash_start_time_ >= led_flash_duration_) {
        set_led(Color::OFF);
        led_fast_flash_timer_ = false;
#ifndef EMSESP_DEBUG
        System::command_format(nullptr, 0); // Execute format operation, unless in debug mode
#endif
    }
}

// set LED on/off or RGB color
// ignores whether the LED is hidden or not (if hide_led_ is set)
void LED::set_led(Color color) {
    if (!led_gpio_ || color == last_color_) {
        return;
    }
    last_color_ = color;

    // RGB lookup table indexed by Color enum (must match enum order in led.h)
    static constexpr uint8_t B              = RGB_LED_BRIGHTNESS;
    static constexpr uint8_t H              = RGB_LED_BRIGHTNESS / 2;
    static constexpr uint8_t rgb_table[][3] = {
        {0, 0, 0}, // OFF
        {B, B, B}, // ON (white)
        {B, 0, 0}, // RED
        {0, B, 0}, // GREEN
        {0, 0, B}, // BLUE
        {B, B, 0}, // YELLOW
        {B, H, 0}, // ORANGE
        {0, B, B}, // CYAN
        {H, 0, H}  // PINK
    };
    static_assert(sizeof(rgb_table) / sizeof(rgb_table[0]) == Color::PINK + 1, "rgb_table must match the Color enum");

    const uint8_t * rgb = rgb_table[color];

    if (led_type_) {
        rgb_led::write(led_gpio_, rgb[0], rgb[1], rgb[2]);
    } else {
        digitalWrite(led_gpio_, color == Color::OFF ? !LED_ON : LED_ON);
    }
}

// set LED custom routine
// For example: /api/system/led?data=red:blink1
// For older non-RGB models, the colour would default to just being on.
bool LED::set_custom_led_routine(const std::string & color, const std::string & pattern) {
    static constexpr struct {
        const char * name;
        Color        value;
    } color_map[] = {
        {"", Color::OFF},
        {"off", Color::OFF},
        {"on", Color::ON},
        {"white", Color::ON},
        {"red", Color::RED},
        {"green", Color::GREEN},
        {"blue", Color::BLUE},
        {"yellow", Color::YELLOW},
        {"orange", Color::ORANGE},
        {"cyan", Color::CYAN},
        {"pink", Color::PINK},
    };

    Color color_type    = Color::OFF;
    bool  color_matched = false;
    for (const auto & entry : color_map) {
        if (color == entry.name) {
            color_type    = entry.value;
            color_matched = true;
            break;
        }
    }
    if (!color_matched) {
        return false;
    }

    // build the steps locally so an unrecognized pattern leaves the current sequence untouched
    Color steps[3] = {Color::OFF, Color::OFF, Color::OFF};

    // blink patterns
    if (pattern == "blink1") {
        steps[0] = color_type;
    } else if (pattern == "blink2") {
        steps[0] = steps[1] = color_type;
    } else if (pattern == "blink3") {
        steps[0] = steps[1] = steps[2] = color_type;

        // special patterns, ignores the user color
    } else if (pattern == "rgb") {
        steps[0] = Color::RED;
        steps[1] = Color::GREEN;
        steps[2] = Color::BLUE;
    } else if (pattern == "cpc") {
        steps[0] = Color::CYAN;
        steps[1] = Color::PINK;
        steps[2] = Color::CYAN;
    } else {
        return false; // pattern not recognized
    }

    color_steps_[0]    = steps[0];
    color_steps_[1]    = steps[1];
    color_steps_[2]    = steps[2];
    is_user_led_blink_ = true; // user routine is active

    // when this is called we want the sequence_led to restart immediately and skip the long pause
    led_long_timer_ = uuid::get_uptime() + HEALTHCHECK_LED_FLASH_FAST_DURATION + 200UL;

    return true;
}

// uses LED to show system health and user-requested LED blinks
// it works in a batch of 3 configured flashes, then a long pause
// the timing is different for user-requested LED blink and for system healthcheck
void LED::sequence_led() {
    // first long pause before we start flashing
    auto current_time = uuid::get_uptime();
    if (led_long_timer_
        && (uint32_t)(current_time - led_long_timer_) >= (is_user_led_blink_ ? HEALTHCHECK_LED_LONG_FAST_DURATION : HEALTHCHECK_LED_LONG_DURATION)) {
        led_short_timer_ = current_time; // start the short timer
        led_long_timer_  = 0;            // stop long timer
        led_flash_step_  = 1;            // enable the short flash timer
    }

    // the flash timer which starts after the long pause
    if (led_flash_step_
        && (uint32_t)(current_time - led_short_timer_) >= (is_user_led_blink_ ? HEALTHCHECK_LED_FLASH_FAST_DURATION : HEALTHCHECK_LED_FLASH_DURATION)) {
        led_long_timer_  = 0; // stop the long timer
        led_short_timer_ = current_time;

        if (++led_flash_step_ == 8) {
            // finished first iteration, reset the whole sequence, turn off LED
            led_long_timer_ = current_time;
            led_flash_step_ = 0;
            set_led(Color::OFF); // turn off the LED

            // if we're running a user-requested LED blink, turn it off and go back to the healthcheck sequence
            if (is_user_led_blink_) {
                is_user_led_blink_    = false;
                previous_healthcheck_ = System::HEALTHCHECK_RESET; // this will force the healthcheck to be checked again
            }
            return;
        }

        // odd steps 3, 5 and 7 show the three configured colors, even steps turn the LED off to make it flash
        set_led(led_flash_step_ % 2 ? color_steps_[(led_flash_step_ - 3) / 2] : Color::OFF);
    }
}

// Start the LED flash timer - duration in seconds
void LED::start_led_fast_flash(uint8_t duration) {
    // Don't start if already running
    if (led_fast_flash_timer_) {
        return;
    }

    // Reset counter and state
    led_flash_start_time_ = uuid::get_uptime();        // current time
    led_flash_duration_   = (uint32_t)duration * 1000; // duration in milliseconds
    led_fast_flash_timer_ = true;                      // it's active
}

} // namespace emsesp