/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
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

#ifndef EMSESP_EMSFACTORY_H_
#define EMSESP_EMSFACTORY_H_

#include <cstdint>
#include <memory> // for unique_ptr
#include <map>

// Forward declaration
namespace emsesp {
class EMSdevice;
}

// Macro for class registration
#define REGISTER_FACTORY(derivedClass, device_type)                                                                                                            \
    namespace {                                                                                                                                                \
    auto registry_##derivedClass = ConcreteEMSFactory<derivedClass>(device_type);                                                                              \
    }

namespace emsesp {

class EMSFactory {
  public:
    virtual ~EMSFactory() = default;

    using FactoryMap = std::map<uint8_t, EMSFactory *>;

    // Register factory object of derived class using the device_type as the unique identifier
    static inline auto registerFactory(const uint8_t device_type, EMSFactory * factory) -> void {
        getRegister()[device_type] = factory;
    }

    // returns all registered classes (really only for debugging)
    static inline auto device_handlers() -> const FactoryMap & {
        return getRegister();
    }

    // Construct derived class returning an unique ptr
    static auto add(const uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * default_name, uint8_t flags, uint8_t brand)
        -> std::unique_ptr<EMSdevice> {
        if (auto * ptr = makeRaw(device_type, device_id, product_id, version, default_name, flags, brand)) {
            return std::unique_ptr<EMSdevice>(ptr);
        }
        return nullptr;
    }

    virtual auto construct(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand) const
        -> EMSdevice * = 0;

  private:
    // Force global variable to be initialized, thus it avoids the "initialization order fiasco"
    static inline auto getRegister() -> FactoryMap & {
        static FactoryMap classRegister{};
        return classRegister;
    }

    // Construct derived class returning a raw pointer
    // find which EMS device it is and use that class
    static auto makeRaw(const uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
        -> EMSdevice * {
        const auto & reg = getRegister();
        const auto   it  = reg.find(device_type);
        return (it != reg.end()) ? it->second->construct(device_type, device_id, product_id, version, name, flags, brand) : nullptr;
    }
};

template <typename DerivedClass>
class ConcreteEMSFactory : EMSFactory {
  public:
    // Register this global object on the EMSFactory register
    explicit ConcreteEMSFactory(const uint8_t device_type) {
        EMSFactory::registerFactory(device_type, this);
    }

    auto construct(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand) const
        -> EMSdevice * {
        return new DerivedClass(device_type, device_id, product_id, version, name, flags, brand);
    }
};

} // namespace emsesp

#endif
