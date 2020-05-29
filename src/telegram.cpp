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

#include "telegram.h"
#include "emsesp.h"

MAKE_PSTR(logger_name, "telegram")

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
uint8_t  EMSbus::ems_bus_id_        = EMSESP_DEFAULT_BUS_ID;
bool     EMSbus::tx_waiting_        = false;
bool     EMSbus::tx_active_         = false;

uuid::log::Logger EMSbus::logger_{F_(logger_name), uuid::log::Facility::CONSOLE};

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
Telegram::Telegram(uint8_t operation, uint8_t src, uint8_t dest, uint16_t type_id, uint8_t offset, uint8_t * data, uint8_t message_length)
    : operation(operation)
    , src(src)
    , dest(dest)
    , type_id(type_id)
    , offset(offset)
    , message_length(message_length) {
    // copy complete telegram data over
    for (uint8_t i = 0; i < message_length; i++) {
        message_data[i] = data[i];
    }
}

// returns telegram's message data bytes in hex
std::string Telegram::to_string() const {
    if (message_length == 0) {
        return read_flash_string(F("<empty>"));
    }

    std::string str(160, '\0');
    char        buffer[4];
    char *      p = &str[0];
    for (uint8_t i = 0; i < this->message_length; i++) {
        Helpers::hextoa(buffer, this->message_data[i]);
        *p++ = buffer[0];
        *p++ = buffer[1];
        *p++ = ' '; // space
    }
    *--p = '\0'; // null terminate just in case, loosing the trailing space

    return str;
}

// returns telegram's full telegram message in hex
std::string Telegram::to_string(const uint8_t * telegram, uint8_t length) const {
    return Helpers::data_to_hex(telegram, length);
}

// validate we have data at the offset (index) requested
// get adjusted index position based on offset
// if offset is 0, it takes the whole telegram. if it's for example 1 it'll show the 2nd data item and
// everything after it
// returns -1 if out of bounds
int8_t Telegram::_getDataPosition(const uint8_t index) const {
    return ((index - offset) >= message_length) ? -1 : (index - offset);
}

// unsigned byte
void Telegram::read_value(uint8_t & param, const uint8_t index) const {
    int8_t pos = _getDataPosition(index);
    if (pos < 0) {
        return;
    }
    param = (uint8_t)message_data[pos];
}

// signed byte
void Telegram::read_value(int8_t & param, const uint8_t index) const {
    int8_t pos = _getDataPosition(index);
    if (pos < 0) {
        return;
    }
    param = (int8_t)message_data[pos];
}

// unsigned short
void Telegram::read_value(uint16_t & param, const uint8_t index) const {
    int8_t pos = _getDataPosition(index);
    if (pos < 0) {
        return;
    }

    uint16_t value = (message_data[pos] << 8) + message_data[pos + 1];

    // check for undefined/unset values, 0x8000, 0x8300, 0x7D00
    if ((value == EMS_VALUE_USHORT_NOTSET) || (value == EMS_VALUE_USHORT_INVALID)) {
        value = EMS_VALUE_USHORT_NOTSET; // make sure we render this right
    }

    param = value;
}

// signed short
void Telegram::read_value(int16_t & param, const uint8_t index) const {
    int8_t pos = _getDataPosition(index);
    if (pos < 0) {
        return;
    }

    int16_t value = (message_data[pos] << 8) + message_data[pos + 1];

    // check for undefined/unset values, 0x8000, 0x8300, 0x7D00
    if ((value == EMS_VALUE_SHORT_NOTSET) || (value == EMS_VALUE_SHORT_INVALID)) {
        value = EMS_VALUE_SHORT_NOTSET; // make sure we render this right
    }

    param = value;
}

// Long
void Telegram::read_value(uint32_t & param, const uint8_t index) const {
    int8_t pos = _getDataPosition(index);
    if (pos < 0) {
        return;
    }

    param = (uint32_t)((message_data[pos] << 16) + (message_data[pos + 1] << 8) + (message_data[pos + 2]));
}

// bit from an unsigned byte
void Telegram::read_value(uint8_t & param, const uint8_t index, const uint8_t bit) const {
    int8_t pos = _getDataPosition(index);
    if (pos < 0) {
        return;
    }

    param = (uint8_t)(((message_data[pos]) >> (bit)) & 0x01);
}

// convert signed short to single 8 byte, for setpoint thermostat temperatures that don't store their temps in 2 bytes
void Telegram::read_value8(int16_t & param, const uint8_t index) const {
    int8_t pos = _getDataPosition(index);
    if (pos < 0) {
        return;
    }

    param = message_data[pos];
}

RxService::QueuedRxTelegram::QueuedRxTelegram(uint16_t id, std::shared_ptr<Telegram> && telegram)
    : id_(id)
    , telegram_(std::move(telegram)) {
}

// empty queue, don't process them
void RxService::flush_rx_queue() {
    rx_telegrams_.clear();
    rx_telegram_id_ = 0;
}

// start and initialize the Rx incoming buffer
void RxService::start() {
    // LOG_DEBUG(F("RxStart"));
    // function not currently used
}

// Rx loop, run as many times as you can
// processes all telegrams on the queue. Assumes there are valid (i.e. CRC checked)
void RxService::loop() {
#ifndef EMSESP_STANDALONE
    // give rx some breathing space
    if ((uuid::get_uptime() - last_rx_check_) < RX_LOOP_WAIT) {
        return;
    }
    last_rx_check_ = uuid::get_uptime();
#endif

    while (!rx_telegrams_.empty()) {
        auto telegram = rx_telegrams_.front().telegram_;

        // rx_telegrams_overflow_ = false;

        (void)EMSESP::process_telegram(telegram); // further process the telegram

        increment_telegram_count(); // increase count

        // remove it from the queue
        // if (!rx_telegrams_overflow_) {
        rx_telegrams_.pop_front();
        // }
    }
}

// add a new rx telegram object
// data is the whole telegram, assuming last byte holds the CRC
// length includes the CRC
// for EMS+ the type_id has the value + 256. We look for these type of telegrams with F7, F9 and FF in 3rd byte
void RxService::add(uint8_t * data, uint8_t length) {
    // validate the CRC
    uint8_t crc = calculate_crc(data, length - 1);
    if (data[length - 1] != crc) {
        LOG_TRACE(F("Rx: %s %s(BAD, CRC %02X != %02X)%s"), Helpers::data_to_hex(data, length).c_str(), COLOR_RED, data[length - 1], crc, COLOR_RESET);
        increment_telegram_error_count();
        return;
    }

    // since it's a valid telegram, work out the ems mask
    // we check the 1st byte, which assumed is the src ID and see if the MSB (8th bit) is set
    // this is used to identify if the protocol should be Junkers/HT3 or Buderus
    // this only happens once with the first rx telegram is processed
    if (ems_mask() == EMS_MASK_UNSET) {
        ems_mask(data[0]);
    }

    // if we're in "trace" and "raw" print out actual telegram
    if (logger_.enabled(Level::TRACE) && EMSESP::trace_raw()) {
        LOG_TRACE(F("Rx: %s"), Helpers::data_to_hex(data, length).c_str());
    }

    // src, dest and offset are always in fixed positions
    uint8_t src    = data[0] & 0x7F; // strip MSB, don't care if its read or write for processing
    uint8_t dest   = data[1] & 0x7F; // strip MSB, don't care if its read or write for processing
    uint8_t offset = data[3];        // offset is always 4th byte

    uint16_t  type_id = 0; // this could be 2 bytes for ems+
    uint8_t * message_data;
    uint8_t   message_length;

    // work out depending on the type where the data message block starts
    if (data[2] < 0xF0 || length < 6) {
        // EMS 1.0
        type_id        = data[2];
        message_data   = data + 4;   // message block starts at 5th byte
        message_length = length - 5; // remove 4 bytes header plus CRC
    } else {
        // EMS 2.0 / EMS+
        if (data[2] == 0xFF) {
            // check for empty data
            // special broadcast telegrams on ems+ have no data values, some even don't have a type ID, e.g. "21 0B FF 00"
            if (length <= 7) {
                message_data   = data; // bogus pointer, will not be used
                message_length = 0;
                if (length <= 5) {
                    type_id = 0; // has also an empty type_id
                } else {
                    type_id = (data[4] << 8) + data[5] + 256;
                }
            } else {
                message_length = length - 7; // remove 6 byte header plus CRC
                message_data   = data + 6;   // message block starts at 7th position
            }
        } else {
            // its F9 or F7
            uint8_t shift = (data[4] != 0xFF); // true (1) if 5th byte is not 0xFF, then telegram is 1 byte longer
            type_id       = (data[5 + shift] << 8) + data[6 + shift] + 256;
            message_data  = data + 6 + shift; // there is a special byte after the typeID which we ignore for now
            if (length <= (9 + shift)) {
                message_length = 0; // special broadcast on ems+ have no data values
            } else {
                message_length = length - (9 + shift);
            }
        }
    }

    // if we don't have a type_id, exit
    if (type_id == 0) {
        return;
    }

    // create the telegram
    auto telegram = std::make_shared<Telegram>(Telegram::Operation::RX, src, dest, type_id, offset, message_data, message_length);

    // check if queue is full
    if (rx_telegrams_.size() >= MAX_RX_TELEGRAMS) {
        // rx_telegrams_overflow_ = true;
        rx_telegrams_.pop_front();
    }

    // add to queue
    LOG_DEBUG(F("New Rx [#%d] telegram, length %d"), rx_telegram_id_, message_length);
    rx_telegrams_.emplace_back(rx_telegram_id_++, std::move(telegram));
}

//
// Tx CODE here
//

TxService::QueuedTxTelegram::QueuedTxTelegram(uint16_t id, std::shared_ptr<Telegram> && telegram)
    : id_(id)
    , telegram_(std::move(telegram)) {
}

// empty queue, don't process
void TxService::flush_tx_queue() {
    tx_telegrams_.clear();
    tx_telegram_id_ = 0;
}

// start and initialize Tx
void TxService::start() {
    // grab the bus ID
    Settings settings;
    ems_bus_id(settings.ems_bus_id());

    // send first Tx request to bus master (boiler) for its registered devices
    // this will be added to the queue and sent during the first tx loop()
    read_request(EMSdevice::EMS_TYPE_UBADevices, EMSdevice::EMS_DEVICE_ID_BOILER);
}

// Tx loop
// here we check if the Tx is not full and report an error
void TxService::loop() {
#ifndef EMSESP_STANDALONE
    if ((uuid::get_uptime() - last_tx_check_) > TX_LOOP_WAIT) {
        last_tx_check_ = uuid::get_uptime();
        if (!tx_active_ && (EMSbus::bus_connected())) {
            LOG_ERROR(F("Tx is not active. Please check connection."));
        }
    }
#endif
}

// sends a 1 byte poll which is our own device ID
void TxService::send_poll() {
    //OG_TRACE(F("Ack %02X"),ems_bus_id() ^ ems_mask());
    EMSuart::send_poll(ems_bus_id() ^ ems_mask());
}

// Process the next telegram on the Tx queue
// This is sent when we receieve a poll request
void TxService::send() {
    // don't process if we don't have a connection to the EMS bus
    // or we're in read-only mode
    if (!bus_connected() || EMSESP::ems_read_only()) {
        return;
    }

    // if there's nothing in the queue to send
    // optionally, send back a poll and quit
    if (tx_telegrams_.empty()) {
        // send_poll(); // TODO commented out poll for now. should add back when stable.
        return;
    }

    // send next telegram in the queue (which is actually a list!)
    send_telegram(tx_telegrams_.front());

    // remove the telegram from the queue
    tx_telegrams_.pop_front();
}

// process a Tx telegram
void TxService::send_telegram(const QueuedTxTelegram & tx_telegram) {
    uint8_t telegram_raw[EMS_MAX_TELEGRAM_LENGTH];

    // build the header
    auto telegram = tx_telegram.telegram_;

    // src - set MSB if its Junkers/HT3
    uint8_t src = telegram->src;
    if (ems_mask() != EMS_MASK_UNSET) {
        src ^= ems_mask();
    }
    telegram_raw[0] = src;

    // dest - for READ the MSB must be set
    // fix the READ or WRITE depending on the operation
    uint8_t dest = telegram->dest;
    if (telegram->operation == Telegram::Operation::TX_READ) {
        dest |= 0x80; // read has 8th bit set for the destination
    }
    telegram_raw[1] = dest;

    uint8_t message_p = 0; // this is the position in the telegram where we want to put our message data

    if (telegram->type_id > 0xFF) {
        // it's EMS 2.0/+
        telegram_raw[2] = 0xFF; // fixed value indicating an extended message
        telegram_raw[3] = telegram->offset;

        // EMS+ has different format for read and write. See https://github.com/proddy/EMS-ESP/wiki/RC3xx-Thermostats
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
        }
    } else {
        // EMS 1.0
        telegram_raw[2] = telegram->type_id;
        telegram_raw[3] = telegram->offset;
        message_p       = 4;
    }

    // add the data to send to to the end of the header
    if (telegram->message_length > EMS_MAX_TELEGRAM_MESSAGE_LENGTH) {
        return; // too big
    }

    for (uint8_t i = 0; i < telegram->message_length; i++) {
        telegram_raw[message_p++] = telegram->message_data[i];
    }

    uint8_t length = message_p;

    remember_tx(telegram_raw, length); // make a copy of it in case we want to re-send it, without the CRC

    telegram_raw[length] = calculate_crc(telegram_raw, length); // generate and append CRC to the end

    length++; // add one since we want to now include the CRC

    LOG_DEBUG(F("Sending %s Tx [#%d], telegram: %s"),
              (telegram->operation == Telegram::Operation::TX_WRITE) ? F("write") : F("read"),
              tx_telegram.id_,
              telegram->to_string(telegram_raw, length).c_str());

    // if we're watching an ID, then always show
    if ((logger_.enabled(Level::TRACE))
        && ((telegram->src == EMSESP::trace_watch_id()) || (telegram->dest == EMSESP::trace_watch_id()) || (telegram->type_id == EMSESP::trace_watch_id()))) {
        logger_.trace(F("Sending %s Tx [#%d], telegram: %s"),
                      (telegram->operation == Telegram::Operation::TX_WRITE) ? F("write") : F("read"),
                      tx_telegram.id_,
                      telegram->to_string(telegram_raw, length).c_str());
    }

    // send the telegram to the UART Tx
    EMSUART_STATUS status = EMSuart::transmit(telegram_raw, length);
    //LOG_TRACE(F("Tx: %s"), Helpers::data_to_hex(telegram_raw, length).c_str());
    if (status != EMS_TX_STATUS_OK) {
        LOG_ERROR(F("Failed to transmit Tx via UART. Error: %s"), status == EMS_TX_WTD_TIMEOUT ? F("Timeout") : F("BRK"));
    }

    tx_waiting(true); // tx now in a wait state
}

// send an array of bytes as a telegram
// we need to calculate the CRC and append it before sending
// this function is fire-and-forget. there are no checks or post-send validations
void TxService::send_telegram(const uint8_t * data, const uint8_t length) {
    uint8_t telegram_raw[EMS_MAX_TELEGRAM_LENGTH];

    for (uint8_t i = 0; i < length; i++) {
        telegram_raw[i] = data[i];
    }
    telegram_raw[length] = calculate_crc(telegram_raw, length); // apppend CRC

    LOG_DEBUG(F("Sending Raw telegram: %s (length=%d)"), Helpers::data_to_hex(telegram_raw, length).c_str(), length);

    // send the telegram to the UART Tx
    EMSUART_STATUS status = EMSuart::transmit(telegram_raw, length);
    //LOG_TRACE(F("Tx: %s"), Helpers::data_to_hex(telegram_raw, length).c_str());
    if (status != EMS_TX_STATUS_OK) {
        LOG_ERROR(F("Failed to transmit Tx via UART. Error: %s"), status == EMS_TX_WTD_TIMEOUT ? F("Timeout") : F("BRK"));
    }
}

// builds a Tx telegram and adds to queue
// given some details like the destination, type, offset and message block
void TxService::add(const uint8_t operation, const uint8_t dest, const uint16_t type_id, const uint8_t offset, uint8_t * message_data, const uint8_t message_length) {
    auto telegram = std::make_shared<Telegram>(operation, ems_bus_id(), dest, type_id, offset, message_data, message_length);
    LOG_DEBUG(F("New Tx [#%d] telegram, length %d"), tx_telegram_id_, message_length);

    // if the queue is full, make room but removing the last one
    if (tx_telegrams_.size() >= MAX_TX_TELEGRAMS) {
        tx_telegrams_.pop_front();
    }

    tx_telegrams_.emplace_back(tx_telegram_id_++, std::move(telegram));
}

// builds a Tx telegram and adds to queue, using only raw data
// length is the length of the whole telegram data
void TxService::add(uint8_t * data, const uint8_t length) {
    uint8_t message_length = length - 4;
    if (!message_length) {
        LOG_ERROR(F("Bad Tx telegram, too short (message length is %d)"), message_length);
        return;
    }

    // build header
    uint8_t   src          = data[0];
    uint8_t   dest         = data[1];
    uint8_t   type_id      = data[2];
    uint8_t   offset       = data[3];
    uint8_t * message_data = data + 4;

    auto telegram = std::make_shared<Telegram>(Telegram::Operation::TX_RAW, src, dest, type_id, offset, message_data, message_length);

    // if the queue is full, make room but removing the last one
    if (tx_telegrams_.size() >= MAX_TX_TELEGRAMS) {
        tx_telegrams_.pop_front();
    }

    LOG_DEBUG(F("New Tx [#%d] telegram, length %d"), tx_telegram_id_, message_length);
    tx_telegrams_.emplace_back(tx_telegram_id_++, std::move(telegram));
}

// send a Tx telegram to request data from an EMS device
void TxService::read_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset) {
    LOG_DEBUG(F("Tx read request to device 0x%02X for type ID 0x%02X"), dest, type_id);

    uint8_t message_data[1] = {EMS_MAX_TELEGRAM_LENGTH}; // request all data, 32 bytes
    add(Telegram::Operation::TX_READ, dest, type_id, offset, message_data, 1);
}

// Send a raw telegram to the bus, telegram is a text string of hex values
void TxService::send_raw(const char * telegram_data) {
    // since the telegram data is a const, make a copy. add 1 to grab the \0 EOS
    char telegram[EMS_MAX_TELEGRAM_LENGTH];
    for (uint8_t i = 0; i < strlen(telegram_data) + 1; i++) {
        telegram[i] = telegram_data[i];
    }

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

    add(data, count + 1); // add to Tx queue
}

// save the last Tx sent, only the telegram, excluding the CRC
void TxService::remember_tx(const uint8_t * data, const uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        telegram_last_[i] = data[i];
    }
    telegram_last_length_ = length;
}

// add last Tx to tx queue and increment count
// returns retry count, or 0 if all done
uint8_t TxService::retry_tx() {
    if (++retry_count_ == MAXIMUM_TX_RETRIES) {
        reset_retry_count(); // give up
    } else {
        add(telegram_last_, telegram_last_length_); // add the last Tx telegram to the tx queue, at the top
    }
    return retry_count_;
}

// checks if a telegram is sent to us matches the last Tx request
// incoming Rx src must match the last Tx dest
// and incoming Rx dest must be us (our ems_bus_id)
// for both src and dest we strip the MSB 8th bit
bool TxService::is_last_tx(const uint8_t src, const uint8_t dest) const {
    // LOG_DEBUG(F("Comparing %02X=%02X , %02X,%02X"), (telegram_last_[1] & 0x7F), (src & 0x7F), (dest & 0x7F), ems_bus_id());
    return (((telegram_last_[1] & 0x7F) == (src & 0x7F)) && ((dest & 0x7F) == ems_bus_id()));
}

// sends a type_id read request to fetch values after a successful Tx write operation
void TxService::post_send_query() {
    if (telegram_last_post_send_query_) {
        uint8_t dest = (telegram_last_[1] & 0x7F);
        LOG_DEBUG(F("Sending post validate read, type ID 0x%02X to dest 0x%02X"), telegram_last_post_send_query_, dest);

        read_request(telegram_last_post_send_query_, dest, 0); // no offset
    }
}

// returns details of the last Tx message that was sent (for debugging)
std::string TxService::last_tx_to_string() const {
    return Helpers::data_to_hex(telegram_last_, telegram_last_length_);
}

} // namespace emsesp
