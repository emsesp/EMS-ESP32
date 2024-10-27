/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  emsesp.org - proddy, MichaelDvP
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

#include <map>
#include <memory> // for unique_ptr

#include "emsdevice.h"

// Macro for class registration
// Anonymous namespace is used to make the definitions here private to the current
// compilation unit (current file). It is equivalent to the old C static keyword.
#define REGISTER_FACTORY(derivedClass, device_type)                                                                                                            \
    namespace {                                                                                                                                                \
    auto registry_##derivedClass = ConcreteEMSFactory<derivedClass>(device_type);                                                                              \
    }

namespace emsesp {

class EMSdevice; // forward declaration, for gcc linking

class EMSFactory {
  public:
    virtual ~EMSFactory() = default;

    // Register factory object of derived class
    // using the device_type as the unique identifier
    static auto registerFactory(const uint8_t device_type, EMSFactory * factory) -> void {
        auto & reg       = EMSFactory::getRegister();
        reg[device_type] = factory;
    }

    using FactoryMap = std::map<uint8_t, EMSFactory *>;

    // returns all registered classes (really only for debugging)
    static auto device_handlers() -> FactoryMap {
        return EMSFactory::getRegister();
    }

    // Construct derived class returning an unique ptr
    static auto add(const uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * default_name, uint8_t flags, uint8_t brand)
        -> std::unique_ptr<EMSdevice> {
        return std::unique_ptr<EMSdevice>(EMSFactory::makeRaw(device_type, device_id, product_id, version, default_name, flags, brand));
    }

    virtual auto construct(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand) const
        -> EMSdevice * = 0;

  private:
    // Force global variable to be initialized, thus it avoids the "initialization order fiasco"
    static auto getRegister() -> FactoryMap & {
        static FactoryMap classRegister{};
        return classRegister;
    }

    // Construct derived class returning a raw pointer
    // find which EMS device it is and use that class
    static auto makeRaw(const uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
        -> EMSdevice * {
        auto it = EMSFactory::getRegister().find(device_type);
        if (it != EMSFactory::getRegister().end()) {
            return it->second->construct(device_type, device_id, product_id, version, name, flags, brand);
        }
        return nullptr;
    }
};

template <typename DerivedClass>
class ConcreteEMSFactory : EMSFactory {
  public:
    // Register this global object on the EMSFactory register
    ConcreteEMSFactory(const uint8_t device_type) {
        EMSFactory::registerFactory(device_type, this);
    }

    auto construct(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand) const
        -> EMSdevice * {
        return new DerivedClass(device_type, device_id, product_id, version, name, flags, brand);
    }
};

} // namespace emsesp

#endif
