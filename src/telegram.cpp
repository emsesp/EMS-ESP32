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

#include "telegram.h"
#include "emsesp.h"

namespace emsesp {

// CRC lookup table with poly 12 for faster checking
const uint8_t ems_crc_table[] = {0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24, 0x26,
                                 0x28, 0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E, 0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E,
                                 0x50, 0x52, 0x54, 0x56, 0x58, 0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C, 0x6E, 0x70, 0x72, 0x74, 0x76,
                                 0x78, 0x7A, 0x7C, 0x7E, 0x80, 0x82, 0x84, 0x86, 0x88, 0x8A, 0x8C, 0x8E, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9A, 0x9C, 0x9E,
                                 0xA0, 0xA2, 0xA4, 0xA6, 0xA8, 0xAA, 0xAC, 0xAE, 0xB0, 0xB2, 0xB4, 0xB6, 0xB8, 0xBA, 0xBC, 0xBE, 0xC0, 0xC2, 0xC4, 0xC6,
                                 0xC8, 0xCA, 0xCC, 0xCE, 0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE,
                                 0xF0, 0xF2, 0xF4, 0xF6, 0xF8, 0xFA, 0xFC, 0xFE, 0x19, 0x1B, 0x1D, 0x1F, 0x11, 0x13, 0x15, 0x17, 0x09, 0x0B, 0x0D, 0x0F,
                                 0x01, 0x03, 0x05, 0x07, 0x39, 0x3B, 0x3D, 0x3F, 0x31, 0x33, 0x35, 0x37, 0x29, 0x2B, 0x2D, 0x2F, 0x21, 0x23, 0x25, 0x27,
                                 0x59, 0x5B, 0x5D, 0x5F, 0x51, 0x53, 0x55, 0x57, 0x49, 0x4B, 0x4D, 0x4F, 0x41, 0x43, 0x45, 0x47, 0x79, 0x7B, 0x7D, 0x7F,
                                 0x71, 0x73, 0x75, 0x77, 0x69, 0x6B, 0x6D, 0x6F, 0x61, 0x63, 0x65, 0x67, 0x99, 0x9B, 0x9D, 0x9F, 0x91, 0x93, 0x95, 0x97,
                                 0x89, 0x8B, 0x8D, 0x8F, 0x81, 0x83, 0x85, 0x87, 0xB9, 0xBB, 0xBD, 0xBF, 0xB1, 0xB3, 0xB5, 0xB7, 0xA9, 0xAB, 0xAD, 0xAF,
                                 0xA1, 0xA3, 0xA5, 0xA7, 0xD9, 0xDB, 0xDD, 0xDF, 0xD1, 0xD3, 0xD5, 0xD7, 0xC9, 0xCB, 0xCD, 0xCF, 0xC1, 0xC3, 0xC5, 0xC7,
                                 0xF9, 0xFB, 0xFD, 0xFF, 0xF1, 0xF3, 0xF5, 0xF7, 0xE9, 0xEB, 0xED, 0xEF, 0xE1, 0xE3, 0xE5, 0xE7};

uint32_t EMSbus::last_bus_activity_ = 0;              // timestamp of last time a valid Rx came in
bool     EMSbus::bus_connected_     = false;          // start assuming the bus hasn't been connected
uint8_t  EMSbus::ems_mask_          = EMS_MASK_UNSET; // unset so its triggered when booting, the its 0x00=buderus, 0x80=junker/ht3
uint8_t  EMSbus::ems_bus_id_        = EMSESP_DEFAULT_EMS_BUS_ID;
uint8_t  EMSbus::tx_mode_           = EMSESP_DEFAULT_TX_MODE;
uint8_t  EMSbus::tx_state_          = Telegram::Operation::NONE;

uuid::log::Logger EMSbus::logger_{F_(telegram), uuid::log::Facility::CONSOLE};

// Calculates CRC checksum using lookup table for speed
// length excludes the last byte (which mainly is the CRC)
uint8_t EMSbus::calculate_crc(const uint8_t * data, const uint8_t length) {
    uint8_t i   = 0;
    uint8_t crc = 0;
    while (i < length) {
        crc = ems_crc_table[crc];
        crc ^= data[i++];
    }
    return crc;
}

// creates a telegram object
// stores header in separate member objects and the rest in the message_data block
Telegram::Telegram(const uint8_t   operation,
                   const uint8_t   src,
                   const uint8_t   dest,
                   const uint16_t  type_id,
                   const uint8_t   offset,
                   const uint8_t * data,
                   const uint8_t   message_length)
    : operation(operation)
    , src(src)
    , dest(dest)
    , type_id(type_id)
    , offset(offset)
    , message_length(message_length) {
    // copy complete telegram data over, preventing buffer overflow
    // faster than using std::move()
    for (uint8_t i = 0; ((i < message_length) && (i < EMS_MAX_TELEGRAM_MESSAGE_LENGTH)); i++) {
        message_data[i] = data[i];
    }
}

// returns telegram as data bytes in hex (excluding CRC)
std::string Telegram::to_string() const {
    uint8_t data[EMS_MAX_TELEGRAM_LENGTH];
    uint8_t length = 0;
    data[0]        = this->src ^ RxService::ems_mask();
    if (this->operation == Telegram::Operation::TX_READ) {
        data[1] = this->dest | 0x80;
        data[4] = this->message_data[0];
        if (this->type_id > 0xFF) {
            data[2] = 0xFF;
            data[5] = (this->type_id >> 8) - 1;
            data[6] = this->type_id & 0xFF;
            length  = 7;
        } else {
            data[2] = this->type_id;
            length  = 5;
        }
    } else {
        data[1] = this->dest;
        if (this->type_id > 0xFF) {
            data[2] = 0xFF;
            data[4] = (this->type_id >> 8) - 1;
            data[5] = this->type_id & 0xFF;
            length  = 6;
        } else {
            data[2] = this->type_id;
            length  = 4;
        }
        for (uint8_t i = 0; i < this->message_length; i++) {
            data[length++] = this->message_data[i];
        }
    }

    return Helpers::data_to_hex(data, length);
}

// returns telegram's message body only, in hex
std::string Telegram::to_string_message() const {
    if (this->message_length == 0) {
        return read_flash_string(F("<empty>"));
    }

    return Helpers::data_to_hex(this->message_data, this->message_length);
}

// checks if we have an Rx telegram that needs processing
void RxService::loop() {
    /*
    while (!rx_telegrams_.empty()) {
        auto telegram = rx_telegrams_.pop().telegram_;
        (void)EMSESP::process_telegram(telegram); // further process the telegram
        increment_telegram_count();               // increase rx count
    }
    */

    while (!rx_telegrams_.empty()) {
        auto telegram = rx_telegrams_.front().telegram_;
        (void)EMSESP::process_telegram(telegram); // further process the telegram
        increment_telegram_count();               // increase rx count
        rx_telegrams_.pop_front();                // remove it from the queue
    }
}

// add a new rx telegram object
// data is the whole telegram, assuming last byte holds the CRC
// length includes the CRC
// for EMS+ the type_id has the value + 256. We look for these type of telegrams with F7, F9 and FF in 3rd byte
void RxService::add(uint8_t * data, uint8_t length) {
    if (length < 2) {
        return;
    }

    // validate the CRC. if it fails then increment the number of corrupt/incomplete telegrams and only report to console/syslog
    uint8_t crc = calculate_crc(data, length - 1);
    if (data[length - 1] != crc) {
        telegram_error_count_++;
        LOG_ERROR(F("Rx: %s (CRC %02X != %02X)"), Helpers::data_to_hex(data, length).c_str(), data[length - 1], crc);
        return;
    }

    // since it's a valid telegram, work out the ems mask
    // we check the 1st byte, which assumed is the src ID and see if the MSB (8th bit) is set
    // this is used to identify if the protocol should be Junkers/HT3 or Buderus
    // this only happens once with the first valid rx telegram is processed
    if (ems_mask() == EMS_MASK_UNSET) {
        ems_mask(data[0]);
    }

    // src, dest and offset are always in fixed positions
    uint8_t src       = data[0] & 0x7F; // strip MSB (HT3 adds it)
    uint8_t dest      = data[1] & 0x7F; // strip MSB, don't care if its read or write for processing
    uint8_t offset    = data[3];        // offset is always 4th byte
    uint8_t operation = (data[1] & 0x80) ? Telegram::Operation::RX_READ : Telegram::Operation::RX;

    uint16_t  type_id;
    uint8_t * message_data;   // where the message block starts
    uint8_t   message_length; // length of the message block, excluding CRC

    // work out depending on the type, where the data message block starts and the message length
    // EMS 1 has type_id always in data[2], if it gets a ems+ inquiry it will reply with FF but short length
    // i.e. sending 0B A1 FF 00 01 D8 20 CRC to a MM10 Mixer (ems1.0), the reply is 21 0B FF 00 CRC
    // see: https://github.com/proddy/EMS-ESP/issues/380#issuecomment-633663007
    if (data[2] != 0xFF || length < 6) {
        // EMS 1.0
        // also handle F7, F9 as EMS 1.0, see https://github.com/proddy/EMS-ESP/issues/109#issuecomment-492781044
        type_id        = data[2];
        message_data   = data + 4;
        message_length = length - 5;
    } else if (data[1] & 0x80) {
        // EMS 2.0 read request
        type_id        = (data[5] << 8) + data[6] + 256;
        message_data   = data + 4; // only data is the requested length
        message_length = 1;
    } else {
        // EMS 2.0 / EMS+
        type_id        = (data[4] << 8) + data[5] + 256;
        message_data   = data + 6;
        message_length = length - 7;
    }

    // if we're watching and "raw" print out actual telegram as bytes to the console
    if (EMSESP::watch() == EMSESP::Watch::WATCH_RAW) {
        uint16_t trace_watch_id = EMSESP::watch_id();
        if ((trace_watch_id == WATCH_ID_NONE) || (type_id == trace_watch_id)
            || ((trace_watch_id < 0x80) && ((src == trace_watch_id) || (dest == trace_watch_id)))) {
            LOG_NOTICE(F("Rx: %s"), Helpers::data_to_hex(data, length).c_str());
        } else if (EMSESP::trace_raw()) {
            LOG_TRACE(F("Rx: %s"), Helpers::data_to_hex(data, length).c_str());
        }
    } else if (EMSESP::trace_raw()) {
        LOG_TRACE(F("Rx: %s"), Helpers::data_to_hex(data, length).c_str());
    }

#ifdef EMSESP_DEBUG
    LOG_DEBUG(F("[DEBUG] New Rx telegram, message length %d"), message_length);
#endif

    // if we don't have a type_id exit,
    // do not exit on empty message, it is checked for toggle fetch
    if (type_id == 0) {
        return;
    }

    // if we receive a hc2.. telegram from 0x19.. match it to master_thermostat if master is 0x18
    src = EMSESP::check_master_device(src, type_id, true);

    // create the telegram
    auto telegram = std::make_shared<Telegram>(operation, src, dest, type_id, offset, message_data, message_length);

    // check if queue is full, if so remove top item to make space
    if (rx_telegrams_.size() >= MAX_RX_TELEGRAMS) {
        rx_telegrams_.pop_front();
    }

    rx_telegrams_.emplace_back(rx_telegram_id_++, std::move(telegram)); // add to queue

    /*
    QueuedRxTelegram qrxt;
    qrxt.telegram_ = std::make_shared<Telegram>(operation, src, dest, type_id, offset, message_data, message_length);
    qrxt.id_       = rx_telegram_id_++;
    rx_telegrams_.push(qrxt);
    */
}

// start and initialize Tx
// send out request to EMS bus for all devices
void TxService::start() {
    // grab the bus ID and tx_mode
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        ems_bus_id(settings.ems_bus_id);
        tx_mode(settings.tx_mode);
    });

    // reset counters
    telegram_read_count(0);
    telegram_write_count(0);
    telegram_fail_count(0);

    // send first Tx request to bus master (boiler) for its registered devices
    // this will be added to the queue and sent during the first tx loop()
    read_request(EMSdevice::EMS_TYPE_UBADevices, EMSdevice::EMS_DEVICE_ID_BOILER);
}

// sends a 1 byte poll which is our own device ID
void TxService::send_poll() {
    //LOG_DEBUG(F("Ack %02X"),ems_bus_id() ^ ems_mask());
    if (tx_mode()) {
        EMSuart::send_poll(ems_bus_id() ^ ems_mask());
    }
}

// Process the next telegram on the Tx queue
// This is sent when we receieve a poll request
void TxService::send() {
    // don't process if we don't have a connection to the EMS bus
    if (!bus_connected()) {
        return;
    }

    // if there's nothing in the queue to transmit or sending should be delayed, send back a poll and quit
    if (tx_telegrams_.empty() || (delayed_send_ && uuid::get_uptime() < delayed_send_)) {
        send_poll();
        return;
    }
    delayed_send_ = 0;

    // auto telegram = tx_telegrams_.pop();    // get the Telegram, also removes from queue

    // if we're in read-only mode (tx_mode 0) forget the Tx call
    if (tx_mode() != 0) {
        // send_telegram(telegram);
        send_telegram(tx_telegrams_.front());
    }

    // auto telegram = tx_telegrams_.pop();
    tx_telegrams_.pop_front(); // remove the telegram from the queue
}

// process a Tx telegram
void TxService::send_telegram(const QueuedTxTelegram & tx_telegram) {
    static uint8_t telegram_raw[EMS_MAX_TELEGRAM_LENGTH];

    // build the header
    auto telegram = tx_telegram.telegram_;

    // src - set MSB if it's Junkers/HT3
    uint8_t src = telegram->src;
    if (ems_mask() != EMS_MASK_UNSET) {
        src ^= ems_mask();
    }
    telegram_raw[0] = src;

    // dest - for READ the MSB must be set
    // fix the READ or WRITE depending on the operation
    uint8_t dest = telegram->dest;

    // check if we have to manipulate the id for thermostats > 0x18
    dest = EMSESP::check_master_device(dest, telegram->type_id, false);

    if (telegram->operation == Telegram::Operation::TX_READ) {
        dest |= 0x80; // read has 8th bit set for the destination
    }
    telegram_raw[1] = dest;

    uint8_t message_p = 0;    // this is the position in the telegram where we want to put our message data
    bool    copy_data = true; // true if we want to copy over the data message block to the end of the telegram header

    if (telegram->type_id > 0xFF) {
        // it's EMS 2.0/+
        telegram_raw[2] = 0xFF; // fixed value indicating an extended message
        telegram_raw[3] = telegram->offset;

        // EMS+ has different format for read and write
        if (telegram->operation == Telegram::Operation::TX_WRITE) {
            // WRITE
            telegram_raw[4] = (telegram->type_id >> 8) - 1; // type, 1st byte, high-byte, subtract 0x100
            telegram_raw[5] = telegram->type_id & 0xFF;     // type, 2nd byte, low-byte
            message_p       = 6;
        } else {
            // READ
            telegram_raw[4] = telegram->message_data[0];    // #bytes to return, which we assume is the only byte in the message block
            telegram_raw[5] = (telegram->type_id >> 8) - 1; // type, 1st byte, high-byte, subtract 0x100
            telegram_raw[6] = telegram->type_id & 0xFF;     // type, 2nd byte, low-byte
            message_p       = 7;
            copy_data       = false; // there are no more data values after the type_id when reading on EMS+
        }
    } else {
        // EMS 1.0
        telegram_raw[2] = telegram->type_id;
        telegram_raw[3] = telegram->offset;
        message_p       = 4;
    }

    if (copy_data) {
        if (telegram->message_length > EMS_MAX_TELEGRAM_MESSAGE_LENGTH) {
            return; // too big
        }

        // add the data to send to to the end of the header
        for (uint8_t i = 0; i < telegram->message_length; i++) {
            telegram_raw[message_p++] = telegram->message_data[i];
        }
    }

    uint8_t length = message_p;

    telegram_last_ = std::make_shared<Telegram>(*telegram); // make a copy of the telegram

    telegram_raw[length] = calculate_crc(telegram_raw, length); // generate and append CRC to the end

    length++; // add one since we want to now include the CRC

    LOG_DEBUG(F("Sending %s Tx [#%d], telegram: %s"),
              (telegram->operation == Telegram::Operation::TX_WRITE) ? F("write") : F("read"),
              tx_telegram.id_,
              Helpers::data_to_hex(telegram_raw, length).c_str());

    set_post_send_query(tx_telegram.validateid_);
    // send the telegram to the UART Tx
    uint16_t status = EMSuart::transmit(telegram_raw, length);

    if (status == EMS_TX_STATUS_ERR) {
        LOG_ERROR(F("Failed to transmit Tx via UART."));
        increment_telegram_fail_count();     // another Tx fail
        tx_state(Telegram::Operation::NONE); // nothing send, tx not in wait state
        return;
    }

    tx_state(telegram->operation); // tx now in a wait state
}

/*
// send an array of bytes as a telegram
// we need to calculate the CRC and append it before sending
// this function is fire-and-forget. there are no checks or post-send validations
void TxService::send_telegram(const uint8_t * data, const uint8_t length) {
    uint8_t telegram_raw[EMS_MAX_TELEGRAM_LENGTH];

    for (uint8_t i = 0; i < length; i++) {
        telegram_raw[i] = data[i];
    }
    telegram_raw[length] = calculate_crc(telegram_raw, length); // apppend CRC

    tx_state(Telegram::Operation::NONE); // no post validation needed

    // send the telegram to the UART Tx
    uint16_t status = EMSuart::transmit(telegram_raw, length);

    if (status == EMS_TX_STATUS_ERR) {
        LOG_ERROR(F("Failed to transmit Tx via UART."));
        increment_telegram_fail_count(); // another Tx fail
    }
}
*/

void TxService::add(const uint8_t  operation,
                    const uint8_t  dest,
                    const uint16_t type_id,
                    const uint8_t  offset,
                    uint8_t *      message_data,
                    const uint8_t  message_length,
                    const uint16_t validateid,
                    const bool     front) {
    auto telegram = std::make_shared<Telegram>(operation, ems_bus_id(), dest, type_id, offset, message_data, message_length);

#ifdef EMSESP_DEBUG
    LOG_DEBUG(F("[DEBUG] New Tx [#%d] telegram, length %d"), tx_telegram_id_, message_length);
#endif

    // if the queue is full, make room but removing the last one
    if (tx_telegrams_.size() >= MAX_TX_TELEGRAMS) {
        tx_telegrams_.pop_front();
    }

    if (front) {
        tx_telegrams_.emplace_front(tx_telegram_id_++, std::move(telegram), false, validateid); // add to front of queue
    } else {
        tx_telegrams_.emplace_back(tx_telegram_id_++, std::move(telegram), false, validateid); // add to back of queue
    }
}

// builds a Tx telegram and adds to queue
// this is used by the retry() function to put the last failed Tx back into the queue
// format is EMS 1.0 (src, dest, type_id, offset, data)
// length is the length of the whole telegram data, excluding the CRC
void TxService::add(uint8_t operation, const uint8_t * data, const uint8_t length, const uint16_t validateid, const bool front) {
    // check length
    if (length < 5) {
        return;
    }

    // build header. src, dest and offset have fixed positions
    uint8_t src    = data[0];
    uint8_t dest   = data[1];
    uint8_t offset = data[3];

    uint16_t        validate_id = validateid;
    uint16_t        type_id;
    const uint8_t * message_data;   // where the message block starts
    uint8_t         message_length; // length of the message block, excluding CRC

    // work out depending on the type, where the data message block starts and the message length
    // same logic as in RxService::add(), but adjusted for no appended CRC
    if (data[2] != 0xFF) {
        // EMS 1.0
        type_id        = data[2];
        message_data   = data + 4;
        message_length = length - 4;
    } else if (data[1] & 0x80) {
        type_id        = (data[5] << 8) + data[6] + 256;
        message_data   = data + 4;
        message_length = 1;
    } else {
        // EMS 2.0 / EMS+
        type_id        = (data[4] << 8) + data[5] + 256;
        message_data   = data + 6;
        message_length = length - 6;
    }

    // if we don't have a type_id or empty data block, exit
    if ((type_id == 0) || (message_length == 0)) {
#ifdef EMSESP_DEBUG
        LOG_DEBUG(F("[DEBUG] Tx telegram type %d failed, length %d"), type_id, message_length);
#endif
        return;
    }

    if (operation == Telegram::Operation::TX_RAW) {
        if (dest & 0x80) {
            operation = Telegram::Operation::TX_READ;
        } else {
            operation = Telegram::Operation::TX_WRITE;
            validate_id = type_id;
        }
        EMSESP::set_read_id(type_id);
    }

    auto telegram = std::make_shared<Telegram>(operation, src, dest, type_id, offset, message_data, message_length); // operation is TX_WRITE or TX_READ

    // if the queue is full, make room but removing the last one
    if (tx_telegrams_.size() >= MAX_TX_TELEGRAMS) {
        tx_telegrams_.pop_front();
    }

#ifdef EMSESP_DEBUG
    LOG_DEBUG(F("[DEBUG] New Tx [#%d] telegram, length %d"), tx_telegram_id_, message_length);
#endif

    /*
    QueuedTxTelegram qtxt;
    qtxt.id_       = tx_telegram_id_++;
    qtxt.retry_    = false;
    qtxt.telegram_ = std::make_shared<Telegram>(operation, ems_bus_id(), dest, type_id, offset, message_data, message_length);
    */

    if (front) {
        // tx_telegrams_.push_front(qtxt); // add to front of queue
        tx_telegrams_.emplace_front(tx_telegram_id_++, std::move(telegram), false, validate_id); // add to front of queue
    } else {
        // tx_telegrams_.push_back(qtxt); // add to back of queue
        tx_telegrams_.emplace_back(tx_telegram_id_++, std::move(telegram), false, validate_id); // add to back of queue
    }
}

// send a Tx telegram to request data from an EMS device
void TxService::read_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset) {
    LOG_DEBUG(F("Tx read request to device 0x%02X for type ID 0x%02X"), dest, type_id);

    uint8_t message_data[1] = {EMS_MAX_TELEGRAM_LENGTH}; // request all data, 32 bytes
    add(Telegram::Operation::TX_READ, dest, type_id, offset, message_data, 1, 0);
}

// Send a raw telegram to the bus, telegram is a text string of hex values
void TxService::send_raw(const char * telegram_data) {
    if (telegram_data == nullptr) {
        return;
    }

    // since the telegram data is a const, make a copy. add 1 to grab the \0 EOS
    char telegram[EMS_MAX_TELEGRAM_LENGTH * 3];
    for (uint8_t i = 0; i < strlen(telegram_data); i++) {
        telegram[i] = telegram_data[i];
    }
    telegram[strlen(telegram_data)] = '\0'; // make sure its terminated

    uint8_t count = 0;
    char *  p;
    char    value[10] = {0};

    uint8_t data[EMS_MAX_TELEGRAM_LENGTH];

    // get first value, which should be the src
    if ((p = strtok(telegram, " ,"))) { // delimiter
        strlcpy(value, p, 10);
        data[0] = (uint8_t)strtol(value, 0, 16);
    }

    // and iterate until end
    while (p != 0) {
        if ((p = strtok(nullptr, " ,"))) {
            strlcpy(value, p, 10);
            uint8_t val   = (uint8_t)strtol(value, 0, 16);
            data[++count] = val;
        }
    }

    if (count == 0) {
        return; // nothing to send
    }

    add(Telegram::Operation::TX_RAW, data, count + 1, 0, true); // add to top/front of Tx queue
}

// add last Tx to tx queue and increment count
// returns retry count, or 0 if all done
void TxService::retry_tx(const uint8_t operation, const uint8_t * data, const uint8_t length) {
    // have we reached the limit? if so, reset count and give up
    if (++retry_count_ > MAXIMUM_TX_RETRIES) {
        reset_retry_count();             // give up
        increment_telegram_fail_count(); // another Tx fail

        LOG_ERROR(F("Last Tx %s operation failed after %d retries. Ignoring request."),
                  (operation == Telegram::Operation::TX_WRITE) ? F("Write") : F("Read"),
                  MAXIMUM_TX_RETRIES);
        return;
    }

#ifdef EMSESP_DEBUG
    LOG_DEBUG(F("[DEBUG] Last Tx %s operation failed. Retry #%d. sent message: %s, received: %s"),
              (operation == Telegram::Operation::TX_WRITE) ? F("Write") : F("Read"),
              retry_count_,
              telegram_last_->to_string().c_str(),
              Helpers::data_to_hex(data, length).c_str());
#endif

    // add to the top of the queue
    if (tx_telegrams_.size() >= MAX_TX_TELEGRAMS) {
        tx_telegrams_.pop_back();
    }

    tx_telegrams_.emplace_front(tx_telegram_id_++, std::move(telegram_last_), true, get_post_send_query());

    /*
    QueuedTxTelegram qtxt;
    qtxt.id_       = tx_telegram_id_++;
    qtxt.retry_    = true; // this time it is a retry
    qtxt.telegram_ = telegram_last_;
    tx_telegrams_.push_front(qtxt); // add to front of queue
    */
}

uint16_t TxService::read_next_tx() {
    // add to the top/front of the queue
    uint8_t message_data[1] = {EMS_MAX_TELEGRAM_LENGTH}; // request all data, 32 bytes
    add(Telegram::Operation::TX_READ, telegram_last_->dest, telegram_last_->type_id, telegram_last_->offset + 25, message_data, 1, 0, true);
    return telegram_last_->type_id;
}

// checks if a telegram is sent to us matches the last Tx request
// incoming Rx src must match the last Tx dest
// and incoming Rx dest must be us (our ems_bus_id)
// for both src and dest we strip the MSB 8th bit
// returns true if the src/dest match the last Tx sent
bool TxService::is_last_tx(const uint8_t src, const uint8_t dest) const {
    return (((telegram_last_->dest & 0x7F) == (src & 0x7F)) && ((dest & 0x7F) == ems_bus_id()));
}

// sends a type_id read request to fetch values after a successful Tx write operation
// unless the post_send_query has a type_id of 0
uint16_t TxService::post_send_query() {
    uint16_t post_typeid = this->get_post_send_query();

    if (post_typeid) {
        uint8_t dest = (this->telegram_last_->dest & 0x7F);
        // when set a value with large offset before and validate on same type, we have to add offset 0, 26, 52, ...
        uint8_t offset          = (this->telegram_last_->type_id == post_typeid) ? ((this->telegram_last_->offset / 26) * 26) : 0;
        uint8_t message_data[1] = {EMS_MAX_TELEGRAM_LENGTH};                                       // request all data, 32 bytes
        this->add(Telegram::Operation::TX_READ, dest, post_typeid, offset, message_data, 1, 0, true); // add to top/front of queue
        // read_request(telegram_last_post_send_query_, dest, 0); // no offset
        LOG_DEBUG(F("Sending post validate read, type ID 0x%02X to dest 0x%02X"), post_typeid, dest);
        set_post_send_query(0); // reset
        // delay the request if we have a different type_id for post_send_query
        delayed_send_ = (this->telegram_last_->type_id == post_typeid) ? 0 : (uuid::get_uptime() + POST_SEND_DELAY);
    }

    return post_typeid;
}

} // namespace emsesp
