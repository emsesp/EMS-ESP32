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
// #include <atomic> // for overflow

#include <uuid/log.h>

#include "helpers.h"

// default values for null values
static constexpr uint8_t EMS_VALUE_BOOL     = 0xFF; // boolean
static constexpr uint8_t EMS_VALUE_BOOL_OFF = 0x00; // boolean false. True can be 0x01 or 0xFF sometimes.

static constexpr uint8_t  EMS_VALUE_BOOL_NOTSET    = 0xFE;   // random number for booleans, that's not 0, 1 or FF
static constexpr uint8_t  EMS_VALUE_UINT_NOTSET    = 0xFF;   // for 8-bit unsigned ints/bytes
static constexpr int8_t   EMS_VALUE_INT_NOTSET     = 0x7F;   // for signed 8-bit ints/bytes
static constexpr int16_t  EMS_VALUE_SHORT_NOTSET   = 0x8300; // -32000: for 2-byte signed shorts
static constexpr uint16_t EMS_VALUE_USHORT_NOTSET  = 0x7D00; //  32000: for 2-byte unsigned shorts
static constexpr uint16_t EMS_VALUE_USHORT_INVALID = 0x8000;
static constexpr int16_t  EMS_VALUE_SHORT_INVALID  = 0x8000;
static constexpr uint32_t EMS_VALUE_ULONG_NOTSET   = 0xFFFFFFFF; // for 3-byte and 4-byte longs
static constexpr uint32_t EMS_VALUE_ULONG_INVALID  = 0x80000000;

static constexpr uint8_t EMS_MAX_TELEGRAM_LENGTH         = 32;                          // max length of a telegram
static constexpr uint8_t EMS_MAX_TELEGRAM_MESSAGE_LENGTH = EMS_MAX_TELEGRAM_LENGTH - 5; // max length of message block

namespace emsesp {

// creates a telegram
// from Rx (receiving one) or Tx for preparing one for sending
class Telegram {
  public:
    Telegram(uint8_t operation, uint8_t src, uint8_t dest, uint16_t type_id, uint8_t offset, uint8_t * message_data, uint8_t message_length);
    ~Telegram() = default;

    const uint8_t  operation; // is Operation mode
    const uint8_t  src;       // device_id
    const uint8_t  dest;      // device_id
    const uint16_t type_id;
    const uint8_t  offset;
    const uint8_t  message_length;

    uint8_t message_data[EMS_MAX_TELEGRAM_MESSAGE_LENGTH];

    std::string to_string() const;
    std::string to_string(const uint8_t * telegram, uint8_t length) const;

    enum Operation : uint8_t {
        RX,
        TX_RAW,
        TX_READ,
        TX_WRITE,
    };

    void read_value(uint16_t & param, const uint8_t index) const;
    void read_value(uint32_t & param, const uint8_t index) const;
    void read_value(uint8_t & param, const uint8_t index, const uint8_t bit) const;
    void read_value(uint8_t & param, const uint8_t index) const;
    void read_value(int16_t & param, const uint8_t index) const;
    void read_value8(int16_t & param, const uint8_t index) const;
    void read_value(int8_t & param, const uint8_t index) const;

  private:
    int8_t _getDataPosition(const uint8_t index) const;
};

class EMSbus {
  public:
    static uuid::log::Logger logger_;

    static constexpr uint8_t EMS_MASK_UNSET   = 0xFF; // EMS bus type (budrus/junkers) hasn't been detected yet
    static constexpr uint8_t EMS_MASK_HT3     = 0x80; // EMS bus type Junkers/HT3
    static constexpr uint8_t EMS_MASK_BUDERUS = 0xFF; // EMS bus type Buderus

    static bool bus_connected() {
#ifndef EMSESP_STANDALONE
        if ((millis() - last_bus_activity_) > EMS_BUS_TIMEOUT) {
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

    static bool tx_waiting() {
        return tx_waiting_;
    }

    static void tx_waiting(bool tx_waiting) {
        tx_waiting_ = tx_waiting;
    }

    static uint8_t calculate_crc(const uint8_t * data, const uint8_t length);

  private:
    static constexpr uint32_t EMS_BUS_TIMEOUT = 30000; // timeout in ms before recognizing the ems bus is offline (30 seconds)

    static uint32_t last_bus_activity_; // timestamp of last time a valid Rx came in
    static bool     bus_connected_;     // start assuming the bus hasn't been connected
    static uint8_t  ems_mask_;          // unset 0x00 buderus 0x80 junker/ht3
    static uint8_t  ems_bus_id_;        // the bus id, which configurable and stored in settings
    static bool     tx_waiting_;        // state of the Tx queue (idle, waiting for ack)
};

class RxService : public EMSbus {
  public:
    static constexpr size_t MAX_RX_TELEGRAMS = 10;

    RxService()  = default;
    ~RxService() = default;

    void start();
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
        QueuedRxTelegram(uint16_t id, uint32_t timestamp, std::shared_ptr<Telegram> && telegram);
        ~QueuedRxTelegram() = default;

        uint16_t                              id_;        // sequential identifier
        uint32_t                              timestamp_; // time it was received
        const std::shared_ptr<const Telegram> telegram_;
    };

    const std::deque<QueuedRxTelegram> queue() const {
        return rx_telegrams_;
    }

  private:
    static constexpr uint32_t RX_LOOP_WAIT   = 800; // delay in processing Rx queue
    uint32_t                  last_rx_check_ = 0;

    size_t maximum_rx_telegrams_ = MAX_RX_TELEGRAMS;

    // std::atomic<bool> rx_telegrams_overflow_{false};
    uint8_t rx_telegram_id_ = 0; // queue counter

    uint16_t telegram_count_       = 0; // # Rx received
    uint16_t telegram_error_count_ = 0; // # Rx CRC errors

    std::deque<QueuedRxTelegram> rx_telegrams_;
};

class TxService : public EMSbus {
  public:
    static constexpr size_t MAX_TX_TELEGRAMS = 20;

    static constexpr uint8_t TX_WRITE_FAIL    = 4;
    static constexpr uint8_t TX_WRITE_SUCCESS = 1;

    TxService()  = default;
    ~TxService() = default;

    void start();
    void send();

    void add(const uint8_t operation, const uint8_t dest, const uint16_t type_id, const uint8_t offset, uint8_t * message_data, const uint8_t message_length);
    void add(uint8_t * data, const uint8_t length);

    void read_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset = 0);

    void send_raw(const char * telegram_data);

    void send_poll();

    void flush_tx_queue();

    void remember_tx(const uint8_t * data, const uint8_t length);

    uint8_t retry_tx();

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

    void increment_telegram_read_count() {
        telegram_read_count_++;
    }

    uint16_t telegram_write_count() const {
        return telegram_write_count_;
    }

    void increment_telegram_write_count() {
        telegram_write_count_++;
    }

    void post_send_query();

    class QueuedTxTelegram {
      public:
        QueuedTxTelegram(uint16_t id, std::shared_ptr<Telegram> && telegram);
        ~QueuedTxTelegram() = default;

        uint16_t                              id_; // sequential identifier
        const std::shared_ptr<const Telegram> telegram_;
    };

    const std::deque<QueuedTxTelegram> queue() const {
        return tx_telegrams_;
    }

    std::string last_tx_to_string() const;


  private:
    static constexpr uint8_t MAXIMUM_TX_RETRIES = 3;

    size_t  maximum_tx_telegrams_ = MAX_TX_TELEGRAMS;
    uint8_t tx_telegram_id_       = 0; // queue counter

    std::deque<QueuedTxTelegram> tx_telegrams_;

    uint8_t  telegram_last_[EMS_MAX_TELEGRAM_LENGTH]; // copy of last telegram
    uint8_t  telegram_last_length_;                   // and its length
    uint16_t telegram_last_post_send_query_;          // which type ID to query after a successful send, to read back the values just written

    const std::shared_ptr<const Telegram> telegram_last;             // copy of last telegram
    uint16_t                              telegram_read_count_  = 0; // # Tx successful reads
    uint16_t                              telegram_write_count_ = 0; // # Tx successful writes

    uint8_t retry_count_ = 0; // count for # Tx retries

    void send_telegram(const QueuedTxTelegram & tx_telegram);
    void send_telegram(const uint8_t * data, const uint8_t length);
};

} // namespace emsesp

#endif