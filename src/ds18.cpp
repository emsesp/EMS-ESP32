/*
 * Dallas support for external settings
 * Copied from Espurna - Copyright (C) 2017-2018 by Xose Pérez <xose dot perez at gmail dot com>
 * 
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 *
 */

#include "ds18.h"

std::vector<ds_device_t> _devices;

DS18::DS18() {
    _wire     = nullptr;
    _count    = 0;
    _gpio     = GPIO_NONE;
    _parasite = 0;
}

DS18::~DS18() {
    if (_wire) {
        delete _wire;
    }
}

// init
void DS18::setup(uint8_t gpio, bool parasite) {
    _gpio     = gpio;
    _parasite = (parasite ? 1 : 0);

    // OneWire
    if (_wire) {
        delete _wire;
    }
    _wire = new OneWire(_gpio);
}

// clear list and scan for devices
uint8_t DS18::scan() {
    _devices.clear();

    uint8_t count = loadDevices(); // start the search

    // If no devices found check again pulling up the line
    if (count == 0) {
        pinMode(_gpio, INPUT_PULLUP);
        count = loadDevices();
    }

    _count = count;

    return count;
}

void DS18::loop() {
    // we either start a conversion or read the scratchpad
    static bool conversion = true;
    if (conversion) {
        _wire->reset();
        _wire->skip();
        _wire->write(DS18_CMD_START_CONVERSION, _parasite);
    } else {
        // Read scratchpads
        for (unsigned char index = 0; index < _devices.size(); index++) {
            if (_wire->reset() == 0) {
                _devices[index].data[0] = _devices[index].data[0] + 1; // Force a CRC check error
                return;
            }

            // Read each scratchpad
            _wire->select(_devices[index].address);
            _wire->write(DS18_CMD_READ_SCRATCHPAD);

            uint8_t data[DS18_DATA_SIZE];
            for (unsigned char i = 0; i < DS18_DATA_SIZE; i++) {
                data[i] = _wire->read();
            }

            if (_wire->reset() != 1) {
                _devices[index].data[0] = _devices[index].data[0] + 1; // Force a CRC check error
                return;
            }

            memcpy(_devices[index].data, data, DS18_DATA_SIZE);
        }
    }

    conversion = !conversion;
}

// return string of the device, with name and address
char * DS18::getDeviceType(char * buffer, unsigned char index) {
    uint8_t size = 128;
    if (index < _count) {
        unsigned char chip_id = chip(index);
        if (chip_id == DS18_CHIP_DS18S20) {
            strlcpy(buffer, "DS18S20", size);
        } else if (chip_id == DS18_CHIP_DS18B20) {
            strlcpy(buffer, "DS18B20", size);
        } else if (chip_id == DS18_CHIP_DS1822) {
            strlcpy(buffer, "DS1822", size);
        } else if (chip_id == DS18_CHIP_DS1825) {
            strlcpy(buffer, "DS1825", size);
        } else {
            strlcpy(buffer, "Unknown", size);
        }
    } else {
        strlcpy(buffer, "invalid", size);
    }

    return buffer;
}

// return string of the device, with name and address
char * DS18::getDeviceID(char * buffer, unsigned char index) {
    uint8_t size = 128;
    if (index < _count) {
        uint8_t * address = _devices[index].address;
        char      a[30]   = {0};
        snprintf(a, sizeof(a), "%02X%02X%02X%02X%02X%02X%02X%02X", address[0], address[1], address[2], address[3], address[4], address[5], address[6], address[7]);

        strlcpy(buffer, a, size);
    } else {
        strlcpy(buffer, "invalid", size);
    }

    return buffer;
}

/*
 * Read sensor values
 * 
 *  Registers:
    byte 0: temperature LSB
    byte 1: temperature MSB
    byte 2: high alarm temp
    byte 3: low alarm temp
    byte 4: DS18S20: store for crc
            DS18B20 & DS1822: configuration register
    byte 5: internal use & crc
    byte 6: DS18S20: COUNT_REMAIN
            DS18B20 & DS1822: store for crc
    byte 7: DS18S20: COUNT_PER_C
            DS18B20 & DS1822: store for crc
    byte 8: SCRATCHPAD_CRC
*/
int16_t DS18::getRawValue(unsigned char index) {
    if (index >= _count) {
        return 0;
    }

    uint8_t * data = _devices[index].data;

    if (OneWire::crc8(data, DS18_DATA_SIZE - 1) != data[DS18_DATA_SIZE - 1]) {
        return DS18_CRC_ERROR;
    }

    int16_t raw = (data[1] << 8) | data[0];
    if (chip(index) == DS18_CHIP_DS18S20) {
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10) {
            raw = (raw & 0xFFF0) + 12 - data[6]; // "count remain" gives full 12 bit resolution
        }
    } else {
        byte cfg = (data[4] & 0x60);
        if (cfg == 0x00) {
            raw = raw & ~7; //  9 bit res, 93.75 ms
        } else if (cfg == 0x20) {
            raw = raw & ~3; // 10 bit res, 187.5 ms
        } else if (cfg == 0x40) {
            raw = raw & ~1; // 11 bit res, 375 ms
                            // 12 bit res, 750 ms
        }
    }

    return raw;
}

// return real value as a float, rounded to 2 decimal places
float DS18::getValue(unsigned char index) {
    int16_t raw_value = getRawValue(index);

    // check if valid
    if ((raw_value == DS18_CRC_ERROR) || (raw_value == DS18_DISCONNECTED)) {
        return (float)DS18_DISCONNECTED;
    }

    // The raw temperature data is in units of sixteenths of a degree,
    // so the value must first be divided by 16 in order to convert it to degrees.
    float new_value = (float)(raw_value / 16.0);

    // round to 2 decimal places
    // https://arduinojson.org/v6/faq/how-to-configure-the-serialization-of-floats/
    return (int)(new_value * 100 + 0.5) / 100.0;
}

// check for a supported DS chip version
bool DS18::validateID(unsigned char id) {
    return (id == DS18_CHIP_DS18S20) || (id == DS18_CHIP_DS18B20) || (id == DS18_CHIP_DS1822) || (id == DS18_CHIP_DS1825);
}

// return the type
unsigned char DS18::chip(unsigned char index) {
    if (index < _count) {
        return _devices[index].address[0];
    }
    return 0;
}

// scan for DS sensors and load into the vector
uint8_t DS18::loadDevices() {
    uint8_t address[8];
    _wire->reset();
    _wire->reset_search();

    while (_wire->search(address)) {
        // Check CRC
        if (_wire->crc8(address, 7) == address[7]) {
            // Check ID
            if (validateID(address[0])) {
                ds_device_t device;
                memcpy(device.address, address, 8);
                _devices.push_back(device);
            }
        }
    }
    return (_devices.size());
}
