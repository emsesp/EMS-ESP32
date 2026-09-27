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

#ifndef EMSESP_LED_H_
#define EMSESP_LED_H_

#include <Arduino.h>
#include <uuid/log.h>

namespace emsesp {

// WS2812B driver replacing arduino-esp32's rgbLedWrite()/neopixelWrite(), which hard-code
// T0H = 400 ns, T1L = 400 ns and have no reset gap. The WS2812B-2020-V6 needs T0H 220-380 ns,
// T1L 580-1000 ns and a reset > 280 us, otherwise it misreads 0-bits as 1-bits (washed-out white).
// Timings used (10 MHz RMT clock, 1 tick = 100 ns) fit both the original WS2812B and the V6:
//
//            sent     old WS2812B    WS2812B-2020-V6
//   T0H     300 ns    250-550 ns     220-380 ns
//   T0L     900 ns    700-1000 ns    580-1000 ns
//   T1H     800 ns    650-950 ns     580-1000 ns
//   T1L     600 ns    300-600 ns     580-1000 ns
//   Reset  >300 us    >50 us         >280 us
//
// Do not mix with rgbLedWrite()/neopixelWrite() on the same pin, as both claim the pin's RMT channel.
namespace rgb_led {

enum class Order : uint8_t { GRB, RGB }; // WS2812B is GRB

// Blocks ~30 us for the transmit, plus up to the reset gap if called again right after the previous update
bool write(uint8_t pin, uint8_t red, uint8_t green, uint8_t blue, Order order = Order::GRB);
void end(uint8_t pin); // release the pin's RMT channel, e.g. before reusing the pin for something else

} // namespace rgb_led

class LED {
  public:
    enum Color : uint8_t {
        OFF    = 0, // 0
        ON     = 1, // 1 - white
        RED    = 2, // 2
        GREEN  = 3, // 3
        BLUE   = 4, // 4
        YELLOW = 5, // 5
        ORANGE = 6, // 6
        CYAN   = 7, // 7
        PINK   = 8  // 8
    };

    void init();
    bool loop(uint8_t healthcheck, bool button_busy);

    void start_led_fast_flash(uint8_t duration); // duration in seconds
    bool set_custom_led_routine(const std::string & color, const std::string & pattern);

  private:
    static uuid::log::Logger logger_;

    void sequence_led();
    void led_fast_flash();
    void reset_led(); // turn the LED back it's default state depending on if it's hidden or not
    void set_led(Color color);

    static constexpr uint32_t HEALTHCHECK_LED_LONG_DURATION       = 1000; // 1 second between flash sequences
    static constexpr uint32_t HEALTHCHECK_LED_LONG_FAST_DURATION  = 500;  // 1/2 second between flash sequences
    static constexpr uint32_t HEALTHCHECK_LED_FLASH_DURATION      = 150;  // 150ms
    static constexpr uint32_t HEALTHCHECK_LED_FLASH_FAST_DURATION = 150;
    static constexpr uint32_t LED_FLASH_INTERVAL_MS               = 100;                      // LED toggle period during factory-reset flash
    static constexpr uint8_t  RGB_LED_BRIGHTNESS                  = 20;                       // 255 is max brightness
    static constexpr uint8_t  LED_ON                              = HIGH;                     // LED on
    static constexpr Color    COLOR_UNSET                         = static_cast<Color>(0xFF); // forces the next set_led() to write

    // led_fast_flash() state
    uint32_t led_flash_start_time_ = 0;
    uint32_t led_flash_duration_   = 0;
    uint32_t last_toggle_time_     = 0;

    // sequence_led() state
    uint32_t led_long_timer_  = 1; // 1 will kick it off immediately
    uint32_t led_short_timer_ = 0;

    // local copies of the application settings
    uint8_t led_gpio_ = 0;
    uint8_t led_type_ = 0;
    bool    hide_led_ = false;

    bool led_fast_flash_timer_ = false;
    bool led_flash_state_      = false;

    bool    last_button_busy_ = false;
    uint8_t led_flash_step_   = 0; // 0 means we're not in the short flash timer

    // set_custom_led_routine() and healthcheck state
    Color color_steps_[3] = {Color::OFF, Color::OFF, Color::OFF};
    Color last_color_     = COLOR_UNSET; // last color written, to skip redundant writes

    // if true, the user has requested a custom LED blink, this always has preference over the button or showing the system health
    bool is_user_led_blink_ = false;

    uint8_t previous_healthcheck_ = 0;
};

} // namespace emsesp

#endif
