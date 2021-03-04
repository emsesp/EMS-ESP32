/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2020  Paul Derbyshire
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

// UART drivers
#if defined(ESP32)
#include "uart/emsuart_esp32.h"
#elif defined(EMSESP_STANDALONE)
#include <emsuart_standalone.h>
#endif

#include <uuid/log.h>

// #include "containers.h"
#include "helpers.h"

#define MAX_RX_TELEGRAMS 10 // size of Rx queue
#define MAX_TX_TELEGRAMS 30 // size of Tx queue

// default values for null values
static constexpr uint8_t EMS_VALUE_BOOL     = 0xFF; // used to mark that something is a boolean
static constexpr uint8_t EMS_VALUE_BOOL_OFF = 0x00; // boolean false
static constexpr uint8_t EMS_VALUE_BOOL_ON  = 0x01; // boolean true. True can be 0x01 or 0xFF sometimes

static constexpr uint8_t  EMS_VALUE_BOOL_NOTSET   = 0xFE;       // random number for booleans, that's not 0, 1 or FF
static constexpr uint8_t  EMS_VALUE_UINT_NOTSET   = 0xFF;       // for 8-bit unsigned ints/bytes
static constexpr int8_t   EMS_VALUE_INT_NOTSET    = 0x7F;       // for signed 8-bit ints/bytes
static constexpr uint16_t EMS_VALUE_USHORT_NOTSET = 0x7D00;     //  32000: for 2-byte unsigned shorts
static constexpr int16_t  EMS_VALUE_SHORT_NOTSET  = 0x7D00;     //  32000: for 2-byte signed shorts
static constexpr uint32_t EMS_VALUE_ULONG_NOTSET  = 0x00FFFFFF; // for 3-byte and 4-byte longs

static constexpr uint8_t EMS_MAX_TELEGRAM_LENGTH         = 32; // max length of a complete EMS telegram
static constexpr uint8_t EMS_MAX_TELEGRAM_MESSAGE_LENGTH = 27; // max length of message block, assuming EMS1.0

namespace emsesp {

class Telegram {
  public:
    Telegram(const uint8_t operation, const uint8_t src, const uint8_t dest, const uint16_t type_id, const uint8_t offset, const uint8_t * message_data, const uint8_t message_length);
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
        RX_READ,
        TX_RAW,
        TX_READ,
        TX_WRITE,
    };

    std::string to_string_message() const;
    std::string to_string() const;

    // reads a bit value from a given telegram position
    bool read_bitvalue(uint8_t & value, const uint8_t index, const uint8_t bit) const {
        uint8_t abs_index = (index - this->offset);
        if (abs_index >= this->message_length) {
            return false; // out of bounds
        }
        uint8_t val = value;
        value       = (uint8_t)(((this->message_data[abs_index]) >> (bit)) & 0x01);
        if (val != value) {
            return true;
        }
        return false;
    }

    // read a value from a telegram. We always store the value, regardless if its garbage
    template <typename Value>
    // assuming negative numbers are stored as 2's-complement
    // https://medium.com/@LeeJulija/how-integers-are-stored-in-memory-using-twos-complement-5ba04d61a56c
    // 2-compliment : https://www.rapidtables.com/convert/number/decimal-to-hex.html
    // https://en.wikipedia.org/wiki/Two%27s_complement
    // s is to override number of bytes read (e.g. use 3 to simulate a uint24_t)
    bool read_value(Value & value, const uint8_t index, uint8_t s = 0) const {
        uint8_t num_bytes = (!s) ? sizeof(Value) : s;
        // check for out of bounds, if so don't modify the value
        if ((index < this->offset) || ((index - this->offset + num_bytes - 1) >= this->message_length)) {
            return false;
        }
        auto val = value;
        value    = 0;
        for (uint8_t i = 0; i < num_bytes; i++) {
            value = (value << 8) + this->message_data[index - this->offset + i]; // shift by byte
        }
        if (val != value) {
            return true;
        }
        return false;
    }

  private:
    int8_t _getDataPosition(const uint8_t index, const uint8_t size) const;
};

class EMSbus {
  public:
    static uuid::log::Logger logger_;

    static constexpr uint8_t EMS_MASK_UNSET     = 0xFF; // EMS bus type (budrus/junkers) hasn't been detected yet
    static constexpr uint8_t EMS_MASK_HT3       = 0x80; // EMS bus type Junkers/HT3
    static constexpr uint8_t EMS_MASK_BUDERUS   = 0xFF; // EMS bus type Buderus
    static constexpr uint8_t EMS_TX_ERROR_LIMIT = 10;   // % limit of failed Tx read/write attempts before showing a warning

    static bool is_ht3() {
        return (ems_mask_ == EMS_MASK_HT3);
    }

    static uint8_t ems_mask() {
        return ems_mask_;
    }

    static void ems_mask(uint8_t ems_mask) {
        ems_mask_ = ems_mask & 0x80; // only keep the MSB (8th bit)
    }

    static uint8_t tx_mode() {
        return tx_mode_;
    }

    static void tx_mode(uint8_t tx_mode) {
        tx_mode_ = tx_mode;
    }

    static uint8_t ems_bus_id() {
        return ems_bus_id_;
    }

    static void ems_bus_id(uint8_t ems_bus_id) {
        ems_bus_id_ = ems_bus_id;
    }

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

    // sets the flag for EMS bus connected
    static void last_bus_activity(uint32_t timestamp) {
        last_bus_activity_ = timestamp;
        bus_connected_     = true;
    }

    static uint8_t tx_state() {
        return tx_state_;
    }
    static void tx_state(uint8_t tx_state) {
        tx_state_ = tx_state;
    }

    static uint8_t calculate_crc(const uint8_t * data, const uint8_t length);

  private:
    static constexpr uint32_t EMS_BUS_TIMEOUT = 30000; // timeout in ms before recognizing the ems bus is offline (30 seconds)

    static uint32_t last_bus_activity_; // timestamp of last time a valid Rx came in
    static bool     bus_connected_;     // start assuming the bus hasn't been connected
    static uint8_t  ems_mask_;          // unset=0xFF, buderus=0x00, junkers/ht3=0x80
    static uint8_t  ems_bus_id_;        // the bus id, which configurable and stored in settings
    static uint8_t  tx_mode_;           // local copy of the tx mode
    static uint8_t  tx_state_;          // state of the Tx line (NONE or waiting on a TX_READ or TX_WRITE)
};

class RxService : public EMSbus {
  public:
    RxService()  = default;
    ~RxService() = default;

    void loop();
    void add(uint8_t * data, uint8_t length);

    uint32_t telegram_count() const {
        return telegram_count_;
    }

    void increment_telegram_count() {
        telegram_count_++;
    }

    uint32_t telegram_error_count() const {
        return telegram_error_count_;
    }

    uint8_t quality() const {
        if (telegram_error_count_ == 0) {
            return 100; // all good, 100%
        }
        uint8_t q = ((float)telegram_error_count_ / telegram_count_ * 100);
        return (q <= EMS_BUS_QUALITY_RX_THRESHOLD ? 100 : 100 - q);
    }

    struct QueuedRxTelegram {
      public:
        const uint16_t                        id_;
        const std::shared_ptr<const Telegram> telegram_;

        ~QueuedRxTelegram() = default;
        QueuedRxTelegram(uint16_t id, std::shared_ptr<Telegram> && telegram)
            : id_(id)
            , telegram_(std::move(telegram)) {
        }
    };

    const std::deque<QueuedRxTelegram> queue() const {
        return rx_telegrams_;
    }

    /*
    struct QueuedRxTelegram {
        uint16_t                        id_;
        std::shared_ptr<const Telegram> telegram_;
    };

    const emsesp::queue<QueuedRxTelegram> queue() const {
        return rx_telegrams_;
    }
    */

  private:
    static constexpr uint8_t EMS_BUS_QUALITY_RX_THRESHOLD = 5; // % threshold before reporting quality issues

    uint8_t                         rx_telegram_id_       = 0; // queue counter
    uint32_t                        telegram_count_       = 0; // # Rx received
    uint32_t                        telegram_error_count_ = 0; // # Rx CRC errors
    std::shared_ptr<const Telegram> rx_telegram;               // the incoming Rx telegram
    std::deque<QueuedRxTelegram>    rx_telegrams_;             // the Rx Queue
    // emsesp::queue<QueuedRxTelegram> rx_telegrams_ = emsesp::queue<QueuedRxTelegram>(MAX_RX_TELEGRAMS); // the Rx Queue
};

class TxService : public EMSbus {
  public:
    static constexpr uint8_t TX_WRITE_FAIL    = 4; // EMS return code for fail
    static constexpr uint8_t TX_WRITE_SUCCESS = 1; // EMS return code for success

    TxService()  = default;
    ~TxService() = default;

    void     start();
    void     send();
    void     add(const uint8_t operation, const uint8_t dest, const uint16_t type_id, const uint8_t offset,  uint8_t * message_data, const uint8_t message_length, const uint16_t validateid, const bool front = false);
    void     add(const uint8_t operation, const uint8_t * data, const uint8_t length, const uint16_t validateid, const bool front = false);
    void     read_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset = 0);
    void     send_raw(const char * telegram_data);
    void     send_poll();
    void     retry_tx(const uint8_t operation, const uint8_t * data, const uint8_t length);
    bool     is_last_tx(const uint8_t src, const uint8_t dest) const;
    uint16_t post_send_query();
    uint16_t read_next_tx();

    uint8_t retry_count() const {
        return retry_count_;
    }

    void reset_retry_count() {
        retry_count_ = 0;
    }

    void set_post_send_query(uint16_t type_id) {
        telegram_last_post_send_query_ = type_id;
    }

    uint16_t get_post_send_query() {
        return telegram_last_post_send_query_;
    }

    uint32_t telegram_read_count() const {
        return telegram_read_count_;
    }

    void telegram_read_count(uint8_t telegram_read_count) {
        telegram_read_count_ = telegram_read_count;
    }

    void increment_telegram_read_count() {
        telegram_read_count_++;
    }

    uint32_t telegram_fail_count() const {
        return telegram_fail_count_;
    }

    void telegram_fail_count(uint8_t telegram_fail_count) {
        telegram_fail_count_ = telegram_fail_count;
    }

    uint8_t quality() const {
        if (telegram_fail_count_ == 0) {
            return 100; // all good, 100%
        }
        return (100 - (uint8_t)(((float)telegram_fail_count_ / telegram_read_count_ * 100)));
    }

    void increment_telegram_fail_count() {
        telegram_fail_count_++;
    }

    uint32_t telegram_write_count() const {
        return telegram_write_count_;
    }

    void telegram_write_count(uint8_t telegram_write_count) {
        telegram_write_count_ = telegram_write_count;
    }

    void increment_telegram_write_count() {
        telegram_write_count_++;
    }

    struct QueuedTxTelegram {
        const uint16_t                        id_;
        const std::shared_ptr<const Telegram> telegram_;
        const bool                            retry_; // true if its a retry
        const uint16_t                        validateid_;

        ~QueuedTxTelegram() = default;
        QueuedTxTelegram(uint16_t id, std::shared_ptr<Telegram> && telegram, bool retry, uint16_t validateid)
            : id_(id)
            , telegram_(std::move(telegram))
            , retry_(retry)
            , validateid_(validateid) {
        }
    };

    const std::deque<QueuedTxTelegram> queue() const {
        return tx_telegrams_;
    }

    /*
    struct QueuedTxTelegram {
        uint16_t                        id_;
        std::shared_ptr<const Telegram> telegram_;
        bool                            retry_; // true if its a retry
    };

    const emsesp::queue<QueuedTxTelegram> queue() const {
        return tx_telegrams_;
    }
    */

#if defined(EMSESP_DEBUG)
    static constexpr uint8_t MAXIMUM_TX_RETRIES = 0; // when compiled with EMSESP_DEBUG don't retry
#else
    static constexpr uint8_t MAXIMUM_TX_RETRIES = 3;
#endif
    static constexpr uint32_t POST_SEND_DELAY = 2000;

  private:
    std::deque<QueuedTxTelegram> tx_telegrams_; // the Tx queue
    // emsesp::queue<QueuedTxTelegram> tx_telegrams_ = emsesp::queue<QueuedTxTelegram>(MAX_TX_TELEGRAMS); // the Tx Queue

    uint32_t telegram_read_count_  = 0; // # Tx successful reads
    uint32_t telegram_write_count_ = 0; // # Tx successful writes
    uint32_t telegram_fail_count_  = 0; // # Tx unsuccessful transmits

    std::shared_ptr<Telegram> telegram_last_;
    uint16_t                  telegram_last_post_send_query_; // which type ID to query after a successful send, to read back the values just written
    uint8_t                   retry_count_  = 0;              // count for # Tx retries
    uint32_t                  delayed_send_ = 0;              // manage delay for post send query

    uint8_t tx_telegram_id_ = 0; // queue counter


    void send_telegram(const QueuedTxTelegram & tx_telegram);
    // void send_telegram(const uint8_t * data, const uint8_t length);
};

} // namespace emsesp

#endif