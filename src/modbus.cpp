/**
 * TODO:
 * - verwendete libs in readme hinzufügen
 */
#include "modbus.h"
#include "emsesp.h"

#include <string>
#include <sstream>
#include <iomanip>

namespace emsesp {

#ifndef EMSESP_STANDALONE
ModbusServerTCPasync Modbus::modbusServer_;
#endif

// parameters
bool     Modbus::enabled        = true;
uint8_t  Modbus::systemServerId = 1;
uint16_t Modbus::port           = 502;
uint8_t  Modbus::maxClients     = 10;
uint32_t Modbus::timeoutMillis  = 10000;

uuid::log::Logger Modbus::logger_{F_(modbus), uuid::log::Facility::DAEMON};

// Map DeviceValue tags to tag types.
std::map<uint8_t, uint8_t> Modbus::tag_to_type{{DeviceValue::TAG_NONE, TAG_TYPE_NONE},
                                               {DeviceValue::TAG_HEARTBEAT, TAG_TYPE_HEARTBEAT},
                                               {DeviceValue::TAG_BOILER_DATA_WW, TAG_TYPE_BOILER_DATA_WW},
                                               {DeviceValue::TAG_DEVICE_DATA, TAG_TYPE_DEVICE_DATA},
                                               {DeviceValue::TAG_DEVICE_DATA_WW, TAG_TYPE_DEVICE_DATA_WW},
                                               {DeviceValue::TAG_HC1, TAG_TYPE_HC},
                                               {DeviceValue::TAG_HC2, TAG_TYPE_HC},
                                               {DeviceValue::TAG_HC3, TAG_TYPE_HC},
                                               {DeviceValue::TAG_HC4, TAG_TYPE_HC},
                                               {DeviceValue::TAG_HC5, TAG_TYPE_HC},
                                               {DeviceValue::TAG_HC6, TAG_TYPE_HC},
                                               {DeviceValue::TAG_HC7, TAG_TYPE_HC},
                                               {DeviceValue::TAG_HC8, TAG_TYPE_HC},
                                               {DeviceValue::TAG_WWC1, TAG_TYPE_WWC},
                                               {DeviceValue::TAG_WWC2, TAG_TYPE_WWC},
                                               {DeviceValue::TAG_WWC3, TAG_TYPE_WWC},
                                               {DeviceValue::TAG_WWC4, TAG_TYPE_WWC},
                                               {DeviceValue::TAG_WWC5, TAG_TYPE_WWC},
                                               {DeviceValue::TAG_WWC6, TAG_TYPE_WWC},
                                               {DeviceValue::TAG_WWC7, TAG_TYPE_WWC},
                                               {DeviceValue::TAG_WWC8, TAG_TYPE_WWC},
                                               {DeviceValue::TAG_WWC9, TAG_TYPE_WWC},
                                               {DeviceValue::TAG_WWC10, TAG_TYPE_WWC},
                                               {DeviceValue::TAG_AHS1, TAG_TYPE_AHS},
                                               {DeviceValue::TAG_HS1, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS2, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS3, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS4, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS5, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS6, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS7, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS8, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS9, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS10, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS11, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS12, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS13, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS14, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS15, TAG_TYPE_HS},
                                               {DeviceValue::TAG_HS16, TAG_TYPE_HS}};

Modbus::Modbus() {
    register_mappings();
}

void Modbus::register_mapping(uint8_t             device_type,
                              uint8_t             device_value_tag_type,
                              const std::string & short_name,
                              uint16_t            modbus_register_offset,
                              uint8_t             modbus_register_count) {
    auto modbusInfo = std::make_shared<EntityModbusInfo>(short_name, modbus_register_offset, modbus_register_count);

#if defined(EMSESP_STANDALONE)
    entityModbusInfoByShortName_[std::make_tuple(device_type, device_value_tag_type, short_name)] = modbusInfo;
#endif

    entityModbusInfoByRegisterOffset_[mk_key(device_type, device_value_tag_type, modbus_register_offset)] = modbusInfo;
}

void Modbus::start() {
    Serial.println("Starting Modbus");
#ifndef EMSESP_STANDALONE
    if (enabled) {
        modbusServer_.registerWorker(systemServerId, READ_INPUT_REGISTER, [this](auto && request) { return handleSystemRead(request); });

        for (uint8_t i = 1; i < 255; i++) {
            if (i != systemServerId) {
                modbusServer_.registerWorker(i, READ_INPUT_REGISTER, [this](auto && request) { return handleRead(request); });
                modbusServer_.registerWorker(i, READ_HOLD_REGISTER, [this](auto && request) { return handleRead(request); });
            }
        }
        modbusServer_.start(port, maxClients, timeoutMillis);
        LOG_INFO("Modbus server with ID %d started on port %d", systemServerId, port);
    }
#else
    LOG_INFO("Modbus deactivated in standalone build.");
#endif
}

#ifndef EMSESP_STANDALONE

template <typename TInputIter>
std::string make_hex_string(TInputIter first, TInputIter last) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    while (first != last)
        ss << std::setw(2) << static_cast<int>(*first++);

    return ss.str();
}

ModbusMessage Modbus::handleSystemRead(const ModbusMessage & request) {
    ModbusMessage response;

    uint16_t start_address = 0;
    uint16_t num_words     = 0;

    request.get(2, start_address);
    request.get(4, num_words);

    LOG_DEBUG("Got request for serverId %d, startAddress %d, numWords %d", request.getServerID(), start_address, num_words);

    if (start_address < 1000) {
        switch (start_address) {
        case 1:
            response.add(request.getServerID());
            response.add(request.getFunctionCode());
            response.add((uint8_t)2);
            response.add((uint16_t)EMSESP::emsdevices.size());
            break;

        default:
            response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        }
    } else if (start_address < 1000 + EMSESP::emsdevices.size()) {
        const auto & dev = EMSESP::emsdevices[start_address - 1000];
        response.add(request.getServerID());
        response.add(request.getFunctionCode());
        response.add((uint8_t)2);
        response.add(dev->device_type());
        response.add(dev->device_id());
    } else {
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
    }

    return response;
}

ModbusMessage Modbus::handleRead(const ModbusMessage & request) {
    ModbusMessage response;

    uint8_t  device_id     = request.getServerID(); // the server ID is the same as the device ID
    uint16_t start_address = 0;
    uint16_t num_words     = 0;

    request.get(2, start_address);
    request.get(4, num_words);

    LOG_DEBUG("Got request for serverId %d, startAddress %d, numWords %d", device_id, start_address, num_words);

    // each register block corresponds to a device value tag
    // TODO check for valid values!
    auto tag             = (uint8_t)(start_address / REGISTER_BLOCK_SIZE);
    auto tag_type        = tag_to_type[tag];
    auto register_offset = start_address - tag * REGISTER_BLOCK_SIZE;

    // TODO linear search for each request is inefficient
    const auto & it =
        std::find_if(EMSESP::emsdevices.begin(), EMSESP::emsdevices.end(), [&](const std::unique_ptr<EMSdevice> & x) { return x->device_id() == device_id; });

    if (it != EMSESP::emsdevices.end()) {
        const auto & dev        = *it;
        auto         modbusInfo = entityModbusInfoByRegisterOffset_[mk_key(dev->device_type(), tag_type, register_offset)];
        // only reading a single value at a time is supported for now
        if (num_words == modbusInfo->registerCount) {
            auto buf = std::vector<uint16_t>(num_words);
            if (dev->copy_raw_value(tag, modbusInfo->short_name, buf)) {
                response.add(request.getServerID());
                response.add(request.getFunctionCode());
                response.add((uint8_t)(num_words * 2));
                for (auto & value : buf)
                    response.add(value);
            } else {
                LOG_ERROR("Unable to read raw device value %s for tag=%d", modbusInfo->short_name.c_str(), (int)tag);
                response.setError(request.getServerID(), request.getFunctionCode(), SERVER_DEVICE_FAILURE);
            }
        } else {
            response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        }
    } else {
        response.setError(request.getServerID(), request.getFunctionCode(), SERVER_ID_MISMATCH);
    }
    /* TEST MODBUS
    if (startAddress == 1 && numWords == 1) {
        response.add(request.getServerID());
        response.add(request.getFunctionCode());
        response.add((uint8_t)2);
        response.add((uint16_t)0xaffe);
        LOG_DEBUG("Response size: %d = %s", response.size(), make_hex_string(response.begin(), response.end()).c_str());
    } else {
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
    }
*/
    return response;
}

#endif

#if defined(EMSESP_STANDALONE)

// return the relative register start offset for a DeviceValue, i.e. the address within the
// register block corresponding to the value's tag type.
int Modbus::getRegisterOffset(const DeviceValue & dv) {
    auto it = entityModbusInfoByShortName_.find(std::make_tuple(dv.device_type, tag_to_type[dv.tag], dv.short_name));
    if (it != entityModbusInfoByShortName_.end()) {
        return it->second->registerOffset;
    }

    return -1;
}

// return the number of registers
int Modbus::getRegisterCount(const DeviceValue & dv) {
    auto it = entityModbusInfoByShortName_.find(std::make_tuple(dv.device_type, tag_to_type[dv.tag], dv.short_name));
    if (it != entityModbusInfoByShortName_.end()) {
        // look up actual size
        return it->second->registerCount;
    }
#if defined(EMSESP_STANDALONE)
    else {
        // guess based on type
        switch (dv.type) {
        case DeviceValue::BOOL:   // 8 bit
        case DeviceValue::SHORT:  // 16 bit
        case DeviceValue::USHORT: // 16 bit
        case DeviceValue::INT:    // 8 bit
        case DeviceValue::UINT:   // 8 bit
        case DeviceValue::ENUM:   // 8 bit
        case DeviceValue::CMD:
            return 1;

        case DeviceValue::ULONG: // 32 bit
        case DeviceValue::TIME:  // 32 bit
            return 2;

        case DeviceValue::STRING:
            break; // impossible to guess, needs to be hardcoded
        }
    }
#endif

    return 0;
}

// return the absolute register start address for a DeviceValue
int Modbus::getRegisterStartAddress(const DeviceValue & dv) {
    return dv.tag * REGISTER_BLOCK_SIZE + getRegisterOffset(dv);
}

#endif

} // namespace emsesp
