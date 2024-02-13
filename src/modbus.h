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

#ifndef EMSESP_STANDALONE
#include <ModbusServerTCPasync.h>
#endif

namespace emsesp {

class Modbus {
  public:
    Modbus();

    static const int REGISTER_BLOCK_SIZE = 1000;

    void start();

#if defined(EMSESP_STANDALONE)
    int  getRegisterOffset(const DeviceValue & dv);
    int  getRegisterCount(const DeviceValue & dv);
    int  getRegisterStartAddress(const DeviceValue & dv);
#endif

  private:
    static uuid::log::Logger logger_;

    struct EntityModbusInfo {
        std::string short_name;
        uint16_t    registerOffset;
        uint16_t    registerCount;

        EntityModbusInfo(const std::string & short_name, uint16_t registerOffset, uint16_t registerCount)
            : short_name(short_name)
            , registerOffset(registerOffset)
            , registerCount(registerCount) {
        }

        EntityModbusInfo(const EntityModbusInfo & other) = default;
    };

    enum DeviceValueTAGType : uint8_t {
        TAG_TYPE_NONE           = DeviceValue::DeviceValueTAG::TAG_NONE,
        TAG_TYPE_HEARTBEAT      = DeviceValue::DeviceValueTAG::TAG_HEARTBEAT,
        TAG_TYPE_BOILER_DATA_WW = DeviceValue::DeviceValueTAG::TAG_BOILER_DATA_WW,
        TAG_TYPE_DEVICE_DATA    = DeviceValue::DeviceValueTAG::TAG_DEVICE_DATA,
        TAG_TYPE_DEVICE_DATA_WW = DeviceValue::DeviceValueTAG::TAG_DEVICE_DATA_WW,
        TAG_TYPE_HC             = DeviceValue::DeviceValueTAG::TAG_HC1,
        TAG_TYPE_WWC            = DeviceValue::DeviceValueTAG::TAG_WWC1,
        TAG_TYPE_AHS            = DeviceValue::DeviceValueTAG::TAG_AHS1,
        TAG_TYPE_HS             = DeviceValue::DeviceValueTAG::TAG_HS1
    };

    static std::map<uint8_t, uint8_t> tag_to_type;

#if defined(EMSESP_STANDALONE)
    std::map<std::tuple<uint8_t /* DeviceType */, uint8_t /* DeviceValueTAGType */, std::string /* short_name */>, std::shared_ptr<EntityModbusInfo>>
        entityModbusInfoByShortName_;
#endif

    std::unordered_map<uint32_t, std::shared_ptr<EntityModbusInfo>> entityModbusInfoByRegisterOffset_;

    static inline uint32_t mk_key(uint8_t device_type, uint8_t device_value_tag_type, uint16_t modbus_register_offset) {
        return ((uint32_t)device_type << 24) | ((uint32_t)device_value_tag_type << 16) | ((uint32_t)modbus_register_offset);
    }

    // implemented in modbus_entity_parameters.cpp
    void register_mappings();

    void register_mapping(uint8_t             device_type,
                          uint8_t             device_value_tag_type,
                          const std::string & short_name,
                          uint16_t            modbus_register_offset,
                          uint8_t             modbus_register_count);

#ifndef EMSESP_STANDALONE
    static ModbusServerTCPasync modbusServer_;
    ModbusMessage               handleSystemRead(const ModbusMessage & request);
    ModbusMessage               handleRead(const ModbusMessage & request);
#endif

    // settings
    static bool     enabled;
    static uint8_t  systemServerId;
    static uint16_t port;
    static uint8_t  maxClients;
    static uint32_t timeoutMillis;
};

} // namespace emsesp

#endif //EMSESP_MODBUS_H_
