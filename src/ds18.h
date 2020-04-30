/*
 * Dallas support for external temperature sensors
 * Copyright (C) 2017-2018 by Xose Pérez <xose dot perez at gmail dot com>
 * 
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 *
 */

#pragma once

#include <OneWire.h>
#include <vector>

#define DS18_CHIP_DS18S20 0x10
#define DS18_CHIP_DS1822 0x22
#define DS18_CHIP_DS18B20 0x28
#define DS18_CHIP_DS1825 0x3B

#define DS18_DATA_SIZE 9
#define DS18_DISCONNECTED -127
#define DS18_CRC_ERROR -126

#define GPIO_NONE 0x99
#define DS18_READ_INTERVAL 2000 // Force sensor read & cache every 2 seconds

#define DS18_CMD_START_CONVERSION 0x44
#define DS18_CMD_READ_SCRATCHPAD 0xBE

typedef struct {
    uint8_t address[8];
    uint8_t data[DS18_DATA_SIZE];
} ds_device_t;

class DS18 {
  public:
    DS18();
    ~DS18();

    void    setup(uint8_t gpio, bool parasite);
    uint8_t scan();
    void    loop();
    char *  getDeviceType(char * s, unsigned char index);
    char *  getDeviceID(char * buffer, unsigned char index);
    float   getValue(unsigned char index);
    int16_t getRawValue(unsigned char index); // raw values, needs / 16

  protected:
    bool          validateID(unsigned char id);
    unsigned char chip(unsigned char index);
    uint8_t       loadDevices();

    OneWire * _wire;
    uint8_t   _count;    // # devices
    uint8_t   _gpio;     // the sensor pin
    uint8_t   _parasite; // parasite mode
};
