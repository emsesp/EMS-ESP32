#if defined(EMSESP_STANDALONE) || defined(EMSESP_TEST)

#ifndef MODBUS_TEST_H
#define MODBUS_TEST_H

#include <cstdint>
#include <vector>

// Mocked ModbusMessage for tests

enum Error : uint8_t {
    SUCCESS                = 0x00,
    ILLEGAL_FUNCTION       = 0x01,
    ILLEGAL_DATA_ADDRESS   = 0x02,
    ILLEGAL_DATA_VALUE     = 0x03,
    SERVER_DEVICE_FAILURE  = 0x04,
    ACKNOWLEDGE            = 0x05,
    SERVER_DEVICE_BUSY     = 0x06,
    NEGATIVE_ACKNOWLEDGE   = 0x07,
    MEMORY_PARITY_ERROR    = 0x08,
    GATEWAY_PATH_UNAVAIL   = 0x0A,
    GATEWAY_TARGET_NO_RESP = 0x0B,
    TIMEOUT                = 0xE0,
    INVALID_SERVER         = 0xE1,
    CRC_ERROR              = 0xE2, // only for Modbus-RTU
    FC_MISMATCH            = 0xE3,
    SERVER_ID_MISMATCH     = 0xE4,
    PACKET_LENGTH_ERROR    = 0xE5,
    PARAMETER_COUNT_ERROR  = 0xE6,
    PARAMETER_LIMIT_ERROR  = 0xE7,
    REQUEST_QUEUE_FULL     = 0xE8,
    ILLEGAL_IP_OR_PORT     = 0xE9,
    IP_CONNECTION_FAILED   = 0xEA,
    TCP_HEAD_MISMATCH      = 0xEB,
    EMPTY_MESSAGE          = 0xEC,
    ASCII_FRAME_ERR        = 0xED,
    ASCII_CRC_ERR          = 0xEE,
    ASCII_INVALID_CHAR     = 0xEF,
    BROADCAST_ERROR        = 0xF0,
    UNDEFINED_ERROR        = 0xFF // otherwise uncovered communication error
};

class ModbusMessage {
  public:
    ModbusMessage() {
    }

    ModbusMessage(std::vector<uint8_t> data) {
        _data = data;
    }

    void get(uint16_t index, uint8_t & value) const {
        value = _data[index];
    }

    void get(uint16_t index, uint16_t & value) const {
        value = (_data[index] << 8) + _data[index + 1];
    }

    void get(uint16_t index, std::vector<uint8_t> & data, uint8_t byte_count) const {
        for (auto i = 0; i < byte_count; i++)
            data.push_back(_data[i + index]);
    }

    void add(uint8_t value) {
        _data.push_back(value);
    }

    void add(uint16_t value) {
        _data.push_back(value >> 8);
        _data.push_back(value & 0xff);
    }

    uint8_t getServerID() const {
        return _data[0];
    }

    uint8_t getFunctionCode() const {
        return _data[1];
    }

    Error getError() const {
        if (_data.size() > 2) {
            if (_data[1] & 0x80) {
                return static_cast<Error>(_data[2]);
            }
        }
        return SUCCESS;
    }

    Error setError(uint8_t serverID, uint8_t functionCode, Error errorCode) {
        _data.reserve(3);
        _data.shrink_to_fit();
        _data.clear();
        add(serverID);
        add(static_cast<uint8_t>((functionCode | 0x80) & 0xFF));
        add(static_cast<uint8_t>(errorCode));
        return SUCCESS;
    }

    std::vector<uint8_t> _data;
};

#endif

#endif
