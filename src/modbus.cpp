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

void Modbus::start() {
    Serial.println("Starting Modbus");
#ifndef EMSESP_STANDALONE
    if (enabled) {
        if (!check_parameter_order()) {
            LOG_ERROR("Unable to enable Modbus - the parameter list order is corrupt. This is a firmware bug.");
            return;
        }

        modbusServer_.registerWorker(systemServerId, READ_INPUT_REGISTER, [this](auto && request) { return handleSystemRead(request); });

        for (uint8_t i = 1; i < 255; i++) {
            if (i != systemServerId) {
                modbusServer_.registerWorker(i, READ_INPUT_REGISTER, [this](auto && request) { return handleRead(request); });
                modbusServer_.registerWorker(i, READ_HOLD_REGISTER, [this](auto && request) { return handleRead(request); });
                modbusServer_.registerWorker(i, WRITE_MULT_REGISTERS, [this](auto && request) { return handleWrite(request); });
            }
        }
        modbusServer_.start(port, maxClients, timeoutMillis);
        LOG_INFO("Modbus server with ID %d started on port %d", systemServerId, port);
    }
#else
    LOG_INFO("Modbus deactivated in standalone build.");
#endif
}

// Check that the Modbus parameters defined in modbus_entity_parameters.cpp are correctly ordered
bool Modbus::check_parameter_order() {
    EntityModbusInfo prev;
    bool             isFirst = true;
    for (const auto & mi : modbus_register_mappings) {
        if (isFirst) {
            isFirst = false;
        } else if (!prev.isLessThan(mi))
            return false;
        prev = mi;
    }
    return true;
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

    LOG_DEBUG("Got read request for serverId %d, startAddress %d, numWords %d", device_id, start_address, num_words);

    // each register block corresponds to a device value tag
    auto tag         = (uint8_t)(start_address / REGISTER_BLOCK_SIZE);
    auto tag_type_it = tag_to_type.find(tag);

    if (tag_type_it == tag_to_type.end()) {
        // invalid register block, does not correspond to an existing tag
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    auto tag_type        = tag_type_it->second;
    auto register_offset = start_address - tag * REGISTER_BLOCK_SIZE;

    const auto & dev_it =
        std::find_if(EMSESP::emsdevices.begin(), EMSESP::emsdevices.end(), [&](const std::unique_ptr<EMSdevice> & x) { return x->device_id() == device_id; });

    if (dev_it != EMSESP::emsdevices.end()) {
        // device not found => invalid server ID
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    const auto & dev = *dev_it;

    // binary search in modbus infos
    auto key = EntityModbusInfoKey(dev->device_type(), tag_type, register_offset);
    auto modbusInfo =
        std::lower_bound(modbus_register_mappings.begin(), modbus_register_mappings.end(), key, [](const EntityModbusInfo & mi, const EntityModbusInfoKey & k) {
            return mi.isLessThan(k);
        });

    if (modbusInfo == modbus_register_mappings.end() || !modbusInfo->equals(key)) {
        // combination of device_type/tag_type/register_offset does not exist
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    // only reading a single value at a time is supported for now
    if (num_words != modbusInfo->registerCount) {
        // number of registers requested does not match actual register count for entity
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    auto buf = std::vector<uint16_t>(num_words);
    if (dev->get_modbus_value(tag, modbusInfo->short_name, buf)) {
        response.add(request.getServerID());
        response.add(request.getFunctionCode());
        response.add((uint8_t)(num_words * 2));
        for (auto & value : buf)
            response.add(value);
    } else {
        LOG_ERROR("Unable to read raw device value %s for tag=%d", modbusInfo->short_name, (int)tag);
        response.setError(request.getServerID(), request.getFunctionCode(), SERVER_DEVICE_FAILURE);
    }

    return response;
}

ModbusMessage Modbus::handleWrite(const ModbusMessage & request) {
    ModbusMessage response;

    uint8_t         device_id     = request.getServerID(); // the server ID is the same as the device ID
    uint16_t        start_address = 0;
    uint16_t        num_words     = 0;
    uint8_t         byte_count    = 0;
    vector<uint8_t> data;

    request.get(2, start_address);
    request.get(4, num_words);
    request.get(6, byte_count);
    request.get(7, data, byte_count);

    LOG_DEBUG("Got write request for serverId %d, startAddress %d, numWords %d, byteCount %d", device_id, start_address, num_words, byte_count);

    // each register block corresponds to a device value tag
    auto tag         = (uint8_t)(start_address / REGISTER_BLOCK_SIZE);
    auto tag_type_it = tag_to_type.find(tag);

    if (tag_type_it == tag_to_type.end()) {
        // invalid register block, does not correspond to an existing tag
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    auto tag_type        = tag_type_it->second;
    auto register_offset = start_address - tag * REGISTER_BLOCK_SIZE;

    const auto & dev_it =
        std::find_if(EMSESP::emsdevices.begin(), EMSESP::emsdevices.end(), [&](const std::unique_ptr<EMSdevice> & x) { return x->device_id() == device_id; });

    if (dev_it != EMSESP::emsdevices.end()) {
        // device not found => invalid server ID
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    const auto & dev = *dev_it;

    // binary search in modbus infos
    auto key = EntityModbusInfoKey(dev->device_type(), tag_type, register_offset);
    auto modbusInfo =
        std::lower_bound(modbus_register_mappings.begin(), modbus_register_mappings.end(), key, [](const EntityModbusInfo & mi, const EntityModbusInfoKey & k) {
            return mi.isLessThan(k);
        });

    if (modbusInfo == modbus_register_mappings.end() || !modbusInfo->equals(key)) {
        // combination of device_type/tag_type/register_offset does not exist
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    // only writing a single value at a time is supported for now
    if (num_words != modbusInfo->registerCount) {
        // number of registers requested does not match actual register count for entity
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    JsonDocument input_doc;
    JsonDocument output_doc;
    JsonObject   input, output;

    dev->modbus_value_to_json(tag, modbusInfo->short_name, data, output);

    auto tag_s = std::string(DeviceValue::DeviceValueTAG_mqtt[tag]);
    auto path  = std::string(modbusInfo->short_name);
    if (!tag_s.empty())
        path += std::string("/") + tag_s;

    // parse and call the command
    input               = input_doc.as<JsonObject>();
    output              = output_doc.to<JsonObject>();
    uint8_t return_code = Command::process(path.c_str(), true, input, output); // modbus is always authenticated

    if (return_code != CommandRet::OK) {
        char error[100];
        if (output.size()) {
            snprintf(error,
                     sizeof(error),
                     "Modbus write command failed with error: %s (%s)",
                     (const char *)output["message"],
                     Command::return_code_string(return_code).c_str());
        } else {
            snprintf(error, sizeof(error), "Modbus write command failed with error code (%s)", Command::return_code_string(return_code).c_str());
        }
        LOG_ERROR(error);
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_VALUE);
    } else {
        // all good
        response.add(request.getServerID());
        response.add(request.getFunctionCode());
        response.add(start_address);
        response.add(num_words);
    }

    return response;
}

#endif

#if defined(EMSESP_STANDALONE)

// return the relative register start offset for a DeviceValue, i.e. the address within the
// register block corresponding to the value's tag type.
int Modbus::getRegisterOffset(const DeviceValue & dv) {
    auto it = std::find_if(modbus_register_mappings.begin(), modbus_register_mappings.end(), [&](const EntityModbusInfo & mi) {
        return mi.device_type == dv.device_type && mi.device_value_tag_type == tag_to_type[dv.tag] && !strcmp(mi.short_name, dv.short_name);
    });
    if (it != modbus_register_mappings.end()) {
        return it->registerOffset;
    }

    return -1;
}

// return the number of registers
int Modbus::getRegisterCount(const DeviceValue & dv) {
    auto it = std::find_if(modbus_register_mappings.begin(), modbus_register_mappings.end(), [&](const EntityModbusInfo & mi) {
        return mi.device_type == dv.device_type && mi.device_value_tag_type == tag_to_type[dv.tag] && !strcmp(mi.short_name, dv.short_name);
    });
    if (it != modbus_register_mappings.end()) {
        // look up actual size
        return it->registerCount;
    } else {
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

    return 0;
}

// return the absolute register start address for a DeviceValue
int Modbus::getRegisterStartAddress(const DeviceValue & dv) {
    return dv.tag * REGISTER_BLOCK_SIZE + getRegisterOffset(dv);
}

#endif

} // namespace emsesp
