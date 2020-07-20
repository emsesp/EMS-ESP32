/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
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

#ifndef EMSESP_TELEGRAM_H
#define EMSESP_TELEGRAM_H

#include <string>
#include <deque>
#include <memory> // for unique ptrs
#include <vector>

// UART drivers
#if defined(ESP8266)
#include "uart/emsuart_esp8266.h"
#elif defined(ESP32)
#include "uart/emsuart_esp32.h"
#elif defined(EMSESP_STANDALONE)
#include <emsuart_standalone.h>
#endif

#include <uuid/log.h>

#include "helpers.h"

// default values for null values
static constexpr uint8_t EMS_VALUE_BOOL     = 0xFF; // boolean
static constexpr uint8_t EMS_VALUE_BOOL_OFF = 0x00; // boolean false. True can be 0x01 or 0xFF sometimes.

static constexpr uint8_t  EMS_VALUE_BOOL_NOTSET   = 0xFE;       // random number for booleans, that's not 0, 1 or FF
static constexpr uint8_t  EMS_VALUE_UINT_NOTSET   = 0xFF;       // for 8-bit unsigned ints/bytes
static constexpr int8_t   EMS_VALUE_INT_NOTSET    = 0x7F;       // for signed 8-bit ints/bytes
static constexpr uint16_t EMS_VALUE_USHORT_NOTSET = 0x7D00;     //  32000: for 2-byte unsigned shorts
static constexpr int16_t  EMS_VALUE_SHORT_NOTSET  = 0x7D00;     //  32000: for 2-byte signed shorts
static constexpr uint32_t EMS_VALUE_ULONG_NOTSET  = 0xFFFFFFFF; // for 3-byte and 4-byte longs

static constexpr uint8_t EMS_MAX_TELEGRAM_LENGTH         = 32; // max length of a complete EMS telegram
static constexpr uint8_t EMS_MAX_TELEGRAM_MESSAGE_LENGTH = 27; // max length of message block, assuming EMS1.0

namespace emsesp {

// creates a telegram
// from Rx (receiving one) or Tx for preparing one for sending
class Telegram {
  public:
    Telegram(const uint8_t   operation,
             const uint8_t   src,
             const uint8_t   dest,
             const uint16_t  type_id,
             const uint8_t   offset,
             const uint8_t * message_data,
             const uint8_t   message_length);
    ~Telegram() = default;

    const uint8_t  operation; // is Operation mode
    const uint8_t  src;       // device_id
    const uint8_t  dest;      // device_id
    const uint16_t type_id;
    const uint8_t  offset;
    const uint8_t  message_length;
    uint8_t        message_data[EMS_MAX_TELEGRAM_MESSAGE_LENGTH];

    enum Operation : uint8_t {
        NONE = 0,
        RX,
        TX_RAW,
        TX_READ,
        TX_WRITE,
    };

    std::string to_string() const;
    std::string to_string(const uint8_t * telegram, uint8_t length) const;

    // reads a bit value from a given telegram position
    void read_bitvalue(uint8_t & value, const uint8_t index, const uint8_t bit) const {
        uint8_t abs_index = (index - offset);
        if (abs_index >= message_length - 1) {
            return; // out of bounds
        }

        value = (uint8_t)(((message_data[abs_index]) >> (bit)) & 0x01);
    }

    // read values from a telegram. We always store the value, regardless if its garbage
    template <typename Value>
    // assuming negative numbers are stored as 2's-complement
    // https://medium.com/@LeeJulija/how-integers-are-stored-in-memory-using-twos-complement-5ba04d61a56c
    // 2-compliment : https://www.rapidtables.com/convert/number/decimal-to-hex.html
    // https://en.wikipedia.org/wiki/Two%27s_complement
    // s is to override number of bytes read (e.g. use 3 to simulat a uint24_t)
    void read_value(Value & value, const uint8_t index, uint8_t s = 0) const {
        uint8_t size      = (!s) ? sizeof(Value) : s;
        int8_t  abs_index = ((index - offset + size - 1) >= message_length - 1) ? -1 : (index - offset);
        if (abs_index < 0) {
            return; // out of bounds, we don't change the value
        }

        value = 0;
        for (uint8_t i = 0; i < size; i++) {
            // shift
            value = (value << 8) + message_data[abs_index + i];
        }
    }


  private:
    int8_t _getDataPosition(const uint8_t index, const uint8_t size) const;
};

class EMSbus {
  public:
    static uuid::log::Logger logger_;

    static constexpr uint8_t EMS_MASK_UNSET   = 0xFF; // EMS bus type (budrus/junkers) hasn't been detected yet
    static constexpr uint8_t EMS_MASK_HT3     = 0x80; // EMS bus type Junkers/HT3
    static constexpr uint8_t EMS_MASK_BUDERUS = 0xFF; // EMS bus type Buderus

    static constexpr uint8_t EMS_TX_ERROR_LIMIT = 10; // % limit of failed Tx read/write attempts before showing a warning

    static bool bus_connected() {
#ifndef EMSESP_STANDALONE
        if ((uuid::get_uptime() - last_bus_activity_) > EMS_BUS_TIMEOUT) {
            bus_connected_ = false;
        }
        return bus_connected_;
#else
        return true;
#endif
    }

    static bool is_ht3() {
        return (ems_mask_ == EMS_MASK_HT3);
    }

    static uint8_t protocol() {
        return ems_mask_;
    }

    static uint8_t ems_mask() {
        return ems_mask_;
    }

    static void ems_mask(uint8_t ems_mask) {
        ems_mask_ = ems_mask & 0x80; // only keep the MSB (8th bit)
    }

    static uint8_t ems_bus_id() {
        return ems_bus_id_;
    }

    static void ems_bus_id(uint8_t ems_bus_id) {
        ems_bus_id_ = ems_bus_id;
    }

    // sets the flag for EMS bus connected
    static void last_bus_activity(uint32_t timestamp) {
        last_bus_activity_ = timestamp;
        bus_connected_     = true;
    }

    static bool tx_active() {
        return tx_active_;
    }
    static void tx_active(bool tx_active) {
        tx_active_ = tx_active;
    }

    static uint8_t tx_waiting() {
        return tx_waiting_;
    }
    static void tx_waiting(uint8_t tx_waiting) {
        tx_waiting_ = tx_waiting;

        // if NONE, then it's been reset which means we have an active Tx
        if ((tx_waiting == Telegram::Operation::NONE) && !(tx_active_)) {
            tx_active_ = true;
        }
    }

    static uint8_t calculate_crc(const uint8_t * data, const uint8_t length);

  private:
    static constexpr uint32_t EMS_BUS_TIMEOUT = 30000; // timeout in ms before recognizing the ems bus is offline (30 seconds)

    static uint32_t last_bus_activity_; // timestamp of last time a valid Rx came in
    static bool     bus_connected_;     // start assuming the bus hasn't been connected
    static uint8_t  ems_mask_;          // unset=0xFF, buderus=0x00, junkers/ht3=0x80
    static uint8_t  ems_bus_id_;        // the bus id, which configurable and stored in settings
    static uint8_t  tx_waiting_;        // state of the Tx line (NONE or waiting on a TX_READ or TX_WRITE)
    static bool     tx_active_;         // is true is we have a working Tx connection
};

class RxService : public EMSbus {
  public:
    static constexpr size_t MAX_RX_TELEGRAMS = 20;

    RxService()  = default;
    ~RxService() = default;

    void loop();

    void add(uint8_t * data, uint8_t length);

    void flush_rx_queue();

    uint16_t telegram_count() const {
        return telegram_count_;
    }

    uint16_t telegram_error_count() const {
        return telegram_error_count_;
    }

    void increment_telegram_count() {
        telegram_count_++;
    }

    void increment_telegram_error_count() {
        telegram_error_count_++;
    }

    class QueuedRxTelegram {
      public:
        QueuedRxTelegram(uint16_t id, std::shared_ptr<Telegram> && telegram);
        ~QueuedRxTelegram() = default;

        uint16_t                              id_; // sequential identifier
        const std::shared_ptr<const Telegram> telegram_;
    };

    const std::deque<QueuedRxTelegram> queue() const {
        return rx_telegrams_;
    }

  private:
    uint32_t last_rx_check_ = 0;

    uint8_t rx_telegram_id_ = 0; // queue counter

    uint16_t telegram_count_       = 0; // # Rx received
    uint16_t telegram_error_count_ = 0; // # Rx CRC errors

    std::deque<QueuedRxTelegram> rx_telegrams_;
};

class TxService : public EMSbus {
  public:
    static constexpr size_t MAX_TX_TELEGRAMS = 30; // size of Tx queue

    static constexpr uint8_t TX_WRITE_FAIL    = 4;
    static constexpr uint8_t TX_WRITE_SUCCESS = 1;

    TxService()  = default;
    ~TxService() = default;

    void start();
    void loop();
    void send();

    void add(const uint8_t  operation,
             const uint8_t  dest,
             const uint16_t type_id,
             const uint8_t  offset,
             uint8_t *      message_data,
             const uint8_t  message_length,
             const bool     front = false);
    void add(const uint8_t operation, const uint8_t * data, const uint8_t length, const bool front = false);

    void read_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset = 0);

    void send_raw(const char * telegram_data);

    void send_poll();

    void flush_tx_queue();

    void retry_tx(const uint8_t operation, const uint8_t * data, const uint8_t length);

    uint8_t retry_count() const {
        return retry_count_;
    }

    void reset_retry_count() {
        retry_count_ = 0;
    }

    bool is_last_tx(const uint8_t src, const uint8_t dest) const;

    void set_post_send_query(uint16_t type_id) {
        telegram_last_post_send_query_ = type_id;
    }

    uint16_t telegram_read_count() const {
        return telegram_read_count_;
    }

    void telegram_read_count(uint8_t telegram_read_count) {
        telegram_read_count_ = telegram_read_count;
    }

    void increment_telegram_read_count() {
        telegram_read_count_++;
    }

    uint16_t telegram_fail_count() const {
        return telegram_fail_count_;
    }

    void telegram_fail_count(uint8_t telegram_fail_count) {
        telegram_fail_count_ = telegram_fail_count;
    }

    void increment_telegram_fail_count() {
        telegram_fail_count_++;
    }

    uint16_t telegram_write_count() const {
        return telegram_write_count_;
    }

    void telegram_write_count(uint8_t telegram_write_count) {
        telegram_write_count_ = telegram_write_count;
    }

    void increment_telegram_write_count() {
        telegram_write_count_++;
    }

    void post_send_query();

    void print_last_tx();

    class QueuedTxTelegram {
      public:
        QueuedTxTelegram(uint16_t id, std::shared_ptr<Telegram> && telegram, bool retry);
        ~QueuedTxTelegram() = default;

        uint16_t                              id_; // sequential identifier
        const std::shared_ptr<const Telegram> telegram_;
        bool                                  retry_; // is a retry
    };

    const std::deque<QueuedTxTelegram> queue() const {
        return tx_telegrams_;
    }

    static constexpr uint8_t MAXIMUM_TX_RETRIES = 3;

  private:
    uint8_t tx_telegram_id_ = 0; // queue counter

    static constexpr uint32_t TX_LOOP_WAIT   = 10000; // when to check if Tx is up and running (10 sec)
    uint32_t                  last_tx_check_ = 0;

    std::deque<QueuedTxTelegram> tx_telegrams_;

    uint16_t telegram_read_count_  = 0; // # Tx successful reads
    uint16_t telegram_write_count_ = 0; // # Tx successful writes
    uint16_t telegram_fail_count_  = 0; // # Tx unsuccessful transmits

    std::shared_ptr<Telegram> telegram_last_;
    uint16_t                  telegram_last_post_send_query_; // which type ID to query after a successful send, to read back the values just written
    uint8_t                   retry_count_ = 0;               // count for # Tx retries

    void send_telegram(const QueuedTxTelegram & tx_telegram);
    void send_telegram(const uint8_t * data, const uint8_t length);
};

} // namespace emsesp

#endif