//
// Created by Michael Heyse on 08.02.24.
//

#ifndef EMSESP_MODBUS_H_
#define EMSESP_MODBUS_H_

#include "helpers.h"
#include "emsdevice.h"
#include "emsdevicevalue.h"

#include <string>
#include <map>
#include <utility>

#if defined(EMSESP_STANDALONE) || defined(EMSESP_TEST)

#include <modbus_test.h>

#endif

#ifndef EMSESP_STANDALONE
#include <ModbusServerTCPasync.h>
#endif

namespace emsesp {

class Modbus {
  public:
    static const int REGISTER_BLOCK_SIZE = 1000;

    void start(uint8_t systemServerId, uint16_t port, uint8_t maxClients, uint32_t timeoutMillis);

#if defined(EMSESP_STANDALONE)
    int getRegisterOffset(const DeviceValue & dv);
    int getRegisterCount(const DeviceValue & dv);
    int getRegisterStartAddress(const DeviceValue & dv);
#endif

  private:
    static uuid::log::Logger logger_;

    struct EntityModbusInfoKey {
        const uint8_t  device_type;
        const uint8_t  device_value_tag_type;
        const uint16_t registerOffset;

        EntityModbusInfoKey(uint8_t deviceType, uint8_t deviceValueTagType, uint16_t registerOffset)
            : device_type(deviceType)
            , device_value_tag_type(deviceValueTagType)
            , registerOffset(registerOffset) {
        }

        bool equals(const EntityModbusInfoKey & other) const {
            return device_type == other.device_type && device_value_tag_type == other.device_value_tag_type && registerOffset == other.registerOffset;
        }

        virtual std::string toString() const {
            return std::string("{ device_type=") + std::to_string(device_type) + "; " + std::string("device_value_tag_type=")
                   + std::to_string(device_value_tag_type) + "; " + std::string("registerOffset=") + std::to_string(registerOffset) + std::string(" }");
        }
    };

    struct EntityModbusInfo {
        const uint8_t      device_type;
        const uint8_t      device_value_tag_type;
        const char * const short_name;
        const uint16_t     registerCount;
        const uint16_t     registerOffset;

        bool equals(const EntityModbusInfoKey & other) const {
            return device_type == other.device_type && device_value_tag_type == other.device_value_tag_type && registerOffset == other.registerOffset;
        }

        bool isLessThan(const EntityModbusInfoKey & other) const {
            return device_type < other.device_type || ((device_type == other.device_type) && (device_value_tag_type < other.device_value_tag_type))
                   || ((device_type == other.device_type) && (device_value_tag_type == other.device_value_tag_type) && (registerOffset < other.registerOffset));
        }

        bool isLessThan(const EntityModbusInfo & other) const {
            return device_type < other.device_type || ((device_type == other.device_type) && (device_value_tag_type < other.device_value_tag_type))
                   || ((device_type == other.device_type) && (device_value_tag_type == other.device_value_tag_type) && (registerOffset < other.registerOffset));
        }

        [[nodiscard]] std::string toString() const {
            return std::string("{") + std::string("device_type=") + std::to_string(device_type) + "; " + std::string("device_value_tag_type=")
                   + std::to_string(device_value_tag_type) + "; " + std::string("short_name=") + std::string(short_name) + "; " + std::string("registerCount=")
                   + std::to_string(registerCount) + "; " + std::string("registerOffset=") + std::to_string(registerOffset) + std::string(" }");
        }
    };

    enum DeviceValueTAGType : int8_t {
        TAG_TYPE_NONE        = DeviceValue::DeviceValueTAG::TAG_NONE,
        TAG_TYPE_DEVICE_DATA = DeviceValue::DeviceValueTAG::TAG_DEVICE_DATA,
        TAG_TYPE_HC          = DeviceValue::DeviceValueTAG::TAG_HC1,
        TAG_TYPE_DHW         = DeviceValue::DeviceValueTAG::TAG_DHW1,
        TAG_TYPE_AHS         = DeviceValue::DeviceValueTAG::TAG_AHS1,
        TAG_TYPE_HS          = DeviceValue::DeviceValueTAG::TAG_HS1
    };

    static std::map<uint8_t, uint8_t> tag_to_type;
    static const EntityModbusInfo     modbus_register_mappings[];
    //static const std::initializer_list<EntityModbusInfo> modbus_register_mappings;

    static bool check_parameter_order();

#ifndef EMSESP_STANDALONE
    static ModbusServerTCPasync modbusServer_;
#endif

#if defined(EMSESP_STANDALONE) || defined(EMSESP_TEST)
  public:
#endif

    static ModbusMessage handleSystemRead(const ModbusMessage & request);
    static ModbusMessage handleRead(const ModbusMessage & request);
    static ModbusMessage handleWrite(const ModbusMessage & request);
};

} // namespace emsesp

#endif //EMSESP_MODBUS_H_
