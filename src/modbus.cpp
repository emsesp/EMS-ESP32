/**
 * TODO: verwendete libs in readme hinzufügen
 */
#include "modbus.h"
#include "modbus_entity_parameters.hpp"
#include "emsesp.h"
#include "emsdevice.h"

#include <string>
#include <sstream>
#include <iomanip>

namespace emsesp {

#ifdef EMSESP_STANDALONE
// no eModbus lib in standalone build
enum FunctionCode : uint8_t { WRITE_HOLD_REGISTER = 0x06, WRITE_MULT_REGISTERS = 0x10 };
#endif

uuid::log::Logger Modbus::logger_{F_(modbus), uuid::log::Facility::DAEMON};

void Modbus::start(uint8_t systemServerId, uint16_t port, uint8_t max_clients, uint32_t timeout) {
#ifndef EMSESP_STANDALONE
    if (!check_parameter_order()) {
        LOG_ERROR("Unable to enable Modbus - the parameter list order is corrupt. This is a firmware bug.");
        return;
    }

    modbusServer_ = new ModbusServerTCPasync();
    modbusServer_->registerWorker(systemServerId, READ_INPUT_REGISTER, [this](auto && request) { return handleSystemRead(request); });

    for (uint8_t i = EMSdevice::DeviceType::BOILER; i < EMSdevice::DeviceType::UNKNOWN; i++) {
        if (i != systemServerId) {
            modbusServer_->registerWorker(i, READ_INPUT_REGISTER, [this](auto && request) { return handleRead(request); });
            modbusServer_->registerWorker(i, READ_HOLD_REGISTER, [this](auto && request) { return handleRead(request); });
            modbusServer_->registerWorker(i, WRITE_HOLD_REGISTER, [this](auto && request) { return handleWrite(request); });
            modbusServer_->registerWorker(i, WRITE_MULT_REGISTERS, [this](auto && request) { return handleWrite(request); });
        }
    }
    modbusServer_->start(port, max_clients, timeout);
    LOG_INFO("Starting Modbus service (ID %d, port %d)", systemServerId, port);
#else
    if (!check_parameter_order()) {
        LOG_ERROR("Unable to enable Modbus - the parameter list order is corrupt. This is a firmware bug.");
    }
    LOG_INFO("Modbus deactivated in standalone build.");
#endif
}

// this is currently never called, just for good measure
void Modbus::stop() {
#ifndef EMSESP_STANDALONE
    modbusServer_->stop();
    //     virtual ~ModbusServerTCPasync() = default;
    // delete modbusServer_;
    modbusServer_ = nullptr;
#endif
}

// Check that the Modbus parameters defined in modbus_entity_parameters.cpp are correctly ordered
bool Modbus::check_parameter_order() {
    EntityModbusInfo const * prev    = nullptr;
    bool                     isFirst = true;
    for (const auto & mi : modbus_register_mappings) {
        if (isFirst) {
            isFirst = false;
        } else if (prev == nullptr) {
            LOG_ERROR("Error checking modbus parameters %s.", mi.short_name);
            return false;
        } else if (!prev->isLessThan(mi)) {
            LOG_ERROR("Error in modbus parameters: %s must be listed before %s.", mi.short_name, prev->short_name);
            return false;
        }
        prev = &mi;
    }
    return true;
}

int8_t Modbus::tag_to_type(int8_t tag) {
    // this coulod even be an array
    switch (tag) {
    case DeviceValue::TAG_NONE:
        return TAG_TYPE_NONE;

    case DeviceValue::TAG_DEVICE_DATA:
        return TAG_TYPE_DEVICE_DATA;

    case DeviceValue::TAG_HC1:
        return TAG_TYPE_HC;

    case DeviceValue::TAG_HC2:
        return TAG_TYPE_HC;

    case DeviceValue::TAG_HC3:
        return TAG_TYPE_HC;

    case DeviceValue::TAG_HC4:
        return TAG_TYPE_HC;

    case DeviceValue::TAG_HC5:
        return TAG_TYPE_HC;

    case DeviceValue::TAG_HC6:
        return TAG_TYPE_HC;

    case DeviceValue::TAG_HC7:
        return TAG_TYPE_HC;

    case DeviceValue::TAG_HC8:
        return TAG_TYPE_HC;

    case DeviceValue::TAG_DHW1:
        return TAG_TYPE_DHW;

    case DeviceValue::TAG_DHW2:
        return TAG_TYPE_DHW;

    case DeviceValue::TAG_DHW3:
        return TAG_TYPE_DHW;

    case DeviceValue::TAG_DHW4:
        return TAG_TYPE_DHW;

    case DeviceValue::TAG_DHW5:
        return TAG_TYPE_DHW;

    case DeviceValue::TAG_DHW6:
        return TAG_TYPE_DHW;

    case DeviceValue::TAG_DHW7:
        return TAG_TYPE_DHW;

    case DeviceValue::TAG_DHW8:
        return TAG_TYPE_DHW;

    case DeviceValue::TAG_DHW9:
        return TAG_TYPE_DHW;

    case DeviceValue::TAG_DHW10:
        return TAG_TYPE_DHW;

    case DeviceValue::TAG_AHS1:
        return TAG_TYPE_AHS;

    case DeviceValue::TAG_HS1:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS2:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS3:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS4:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS5:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS6:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS7:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS8:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS9:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS10:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS11:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS12:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS13:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS14:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS15:
        return TAG_TYPE_HS;

    case DeviceValue::TAG_HS16:
        return TAG_TYPE_HS;

    default:
        return INVALID_TAG_TYPE;
    }
}

/* DEBUG
template <typename TInputIter>
std::string make_hex_string(TInputIter first, TInputIter last) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    while (first != last)
        ss << std::setw(2) << static_cast<int>(*first++);

    return ss.str();
}
*/

/**
 *
 * @param request
 * @return
 */
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

    uint8_t  device_type   = request.getServerID(); // the server ID is the same as the device type
    uint16_t start_address = 0;
    uint16_t num_words     = 0;

    request.get(2, start_address);
    request.get(4, num_words);

    LOG_DEBUG("Got read request for serverId %d, startAddress %d, numWords %d", device_type, start_address, num_words);

    // each register block corresponds to a device value tag
    auto tag      = (uint8_t)(start_address / REGISTER_BLOCK_SIZE);
    auto tag_type = tag_to_type(tag);

    if (tag_type == INVALID_TAG_TYPE) {
        // invalid register block, does not correspond to an existing tag
        LOG_ERROR("invalid register block, does not correspond to an existing tag");
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    auto register_offset = start_address - tag * REGISTER_BLOCK_SIZE;

    // binary search in modbus infos
    auto key = EntityModbusInfoKey(device_type, tag_type, register_offset);

    const auto & modbusInfo = std::lower_bound(std::begin(modbus_register_mappings),
                                               std::end(modbus_register_mappings),
                                               key,
                                               [](const EntityModbusInfo & a, const EntityModbusInfoKey & b) { return a.isLessThan(b); });


    if (modbusInfo == std::end(modbus_register_mappings) || !modbusInfo->equals(key)) {
        // combination of device_type/tag_type/register_offset does not exist
        LOG_ERROR("combination of device_type/tag_type/register_offset does not exist");
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    // only reading a single value at a time is supported for now
    if (num_words != modbusInfo->registerCount) {
        // number of registers requested does not match actual register count for entity
        LOG_ERROR("number of registers requested does not match actual register count for entity");
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    auto buf        = std::vector<uint16_t>(num_words);
    int  error_code = -1;
    for (const auto & emsdevice : EMSESP::emsdevices) {
        if (emsdevice->device_type() == device_type) {
            error_code = emsdevice->get_modbus_value(tag, modbusInfo->short_name, buf);
            if (!error_code) {
                break;
            }
        }
    }
    if (error_code) {
        if (uuid::get_uptime_sec() > 60 || error_code < -2) { // suppress not found messages for the first minute
            LOG_ERROR("Unable to read raw device value %s for tag=%d - error_code = %d", modbusInfo->short_name, (int)tag, error_code);
        }
        response.setError(request.getServerID(), request.getFunctionCode(), SERVER_DEVICE_FAILURE);
        return response;
    }

    response.add(request.getServerID());
    response.add(request.getFunctionCode());
    response.add((uint8_t)(num_words * 2));
    for (auto & value : buf)
        response.add(value);

    return response;
}

ModbusMessage Modbus::handleWrite(const ModbusMessage & request) {
    ModbusMessage response;

    uint8_t              device_type   = request.getServerID(); // the server ID is the same as the device type
    uint8_t              function_code = request.getFunctionCode();
    uint16_t             start_address = 0;
    uint16_t             num_words     = 0;
    uint8_t              byte_count    = 0;
    std::vector<uint8_t> data;

    if (function_code == WRITE_MULT_REGISTERS) {
        request.get(2, start_address);
        request.get(4, num_words);
        request.get(6, byte_count);
        request.get(7, data, byte_count);
    } else if (function_code == WRITE_HOLD_REGISTER) {
        num_words  = 1;
        byte_count = 2;
        request.get(2, start_address);
        request.get(4, data, byte_count);
    } else {
        LOG_ERROR("Function code %d is not implemented", function_code);
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_FUNCTION);
        return response;
    }

    LOG_DEBUG("Got write request for serverId %d, startAddress %d, numWords %d, byteCount %d", device_type, start_address, num_words, byte_count);

    // each register block corresponds to a device value tag
    auto tag      = (uint8_t)(start_address / REGISTER_BLOCK_SIZE);
    auto tag_type = tag_to_type(tag);

    if (tag_type == INVALID_TAG_TYPE) {
        // invalid register block, does not correspond to an existing tag
        LOG_ERROR("invalid register block (%d), does not correspond to an existing tag", tag);
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    auto register_offset = start_address - tag * REGISTER_BLOCK_SIZE;

    LOG_DEBUG("Tag %d, offset %d", tag, register_offset);


    // binary search in modbus infos
    auto key        = EntityModbusInfoKey(device_type, tag_type, register_offset);
    auto modbusInfo = std::lower_bound(std::begin(modbus_register_mappings),
                                       std::end(modbus_register_mappings),
                                       key,
                                       [](const EntityModbusInfo & mi, const EntityModbusInfoKey & k) { return mi.isLessThan(k); });

    if (modbusInfo == std::end(modbus_register_mappings) || !modbusInfo->equals(key)) {
        // combination of device_type/tag_type/register_offset does not exist
        LOG_ERROR("combination of device_type (%d)/tag_type (%d)/register_offset (%d) does not exist",
                  key.device_type,
                  key.device_value_tag_type,
                  key.registerOffset);
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    LOG_DEBUG("Writing to entity %s", modbusInfo->short_name);

    // only writing a single value at a time is supported for now
    if (num_words != modbusInfo->registerCount) {
        // number of registers requested does not match actual register count for entity
        LOG_ERROR("number of registers (%d) requested does not match actual register count (%d) for entity", num_words, modbusInfo->registerCount);
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    JsonDocument input_doc;
    JsonObject   input      = input_doc.to<JsonObject>();
    int          error_code = -1;
    for (const auto & emsdevice : EMSESP::emsdevices) {
        if (emsdevice->device_type() == device_type) {
            error_code = emsdevice->modbus_value_to_json(tag, modbusInfo->short_name, data, input);
            if (!error_code) {
                break;
            }
        }
    }
    if (error_code) {
        // error getting modbus value as json
        LOG_ERROR("error getting modbus value as json, error code = %d", error_code);
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
        return response;
    }

    std::string path;
    if (tag < DeviceValueTAG::TAG_HC1) {
        path = std::string("api/") + std::string(EMSdevice::device_type_2_device_name(device_type)) + "/" + modbusInfo->short_name;
    } else {
        path = std::string("api/") + std::string(EMSdevice::device_type_2_device_name(device_type)) + "/" + EMSdevice::tag_to_mqtt(tag) + "/"
               + modbusInfo->short_name;
    }

    LOG_DEBUG("path: %s\n", path.c_str());

    std::string inputStr;
    serializeJson(input, inputStr);
    LOG_DEBUG("input: %s\n", inputStr.c_str());

    JsonDocument output_doc;
    JsonObject   output      = output_doc.to<JsonObject>();
    uint8_t      return_code = Command::process(path.c_str(), true, input, output); // modbus is always authenticated

    if (return_code != CommandRet::OK) {
        char error[100];
        if (output.size()) {
            snprintf(error,
                     sizeof(error),
                     "Modbus write command failed with error %s (%s)",
                     (const char *)output["message"],
                     Command::return_code_string(return_code));
        } else {
            snprintf(error, sizeof(error), "Modbus write command failed with error code (%s)", Command::return_code_string(return_code));
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

#if defined(EMSESP_STANDALONE)
// return the relative register start offset for a DeviceValue, i.e. the address within the
// register block corresponding to the value's tag type.
int Modbus::getRegisterOffset(const DeviceValue & dv) {
    auto it = std::find_if(std::begin(modbus_register_mappings), std::end(modbus_register_mappings), [&](const EntityModbusInfo & mi) {
        return mi.device_type == dv.device_type && mi.device_value_tag_type == tag_to_type(dv.tag) && !strcmp(mi.short_name, dv.short_name);
    });
    if (it != std::end(modbus_register_mappings)) {
        return it->registerOffset;
    }

    return -1;
}

// return the number of registers
int Modbus::getRegisterCount(const DeviceValue & dv) {
    auto it = std::find_if(std::begin(modbus_register_mappings), std::end(modbus_register_mappings), [&](const EntityModbusInfo & mi) {
        return mi.device_type == dv.device_type && mi.device_value_tag_type == tag_to_type(dv.tag) && !strcmp(mi.short_name, dv.short_name);
    });
    if (it != std::end(modbus_register_mappings)) {
        // look up actual size
        return it->registerCount;
    } else {
        // guess based on type
        switch (dv.type) {
        case DeviceValue::BOOL: // 8 bit
        case DeviceValue::INT8:
        case DeviceValue::UINT8:
        case DeviceValue::INT16:
        case DeviceValue::UINT16:
        case DeviceValue::ENUM: // 8 bit
            return 1;

        case DeviceValue::UINT24:
        case DeviceValue::UINT32:
        case DeviceValue::TIME: // 32 bit
            return 2;

        case DeviceValue::CMD: {
            // calculate a sensible register size from min, max and numeric_operator
            uint32_t num_values    = std::max(dv.max, (uint32_t)abs(dv.min));
            int      num_registers = 0;

            if (num_values <= (1L << 8))
                num_registers = 1;
            else if (num_values <= (1L << 16))
                num_registers = 2;
            else if (num_values <= (1L << 32))
                num_registers = 4;
            else
                LOG_ERROR("num_registers is too big to be encoded with modbus registers");

            LOG_DEBUG("Value for CMD '%s' can take on %ld values and is encoded in %d registers", dv.short_name, num_values, num_registers);

            return num_registers;
        }

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
