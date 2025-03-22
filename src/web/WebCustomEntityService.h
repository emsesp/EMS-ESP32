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
#include "../core/telegram.h"

#ifndef WebCustomEntityService_h
#define WebCustomEntityService_h

#define EMSESP_CUSTOMENTITY_FILE "/config/emsespEntity.json"
#define EMSESP_CUSTOMENTITY_SERVICE_PATH "/rest/customEntities" // GET and POST

namespace emsesp {

class CustomEntityItem {
  public:
    uint8_t     id;
    uint8_t     device_id;
    uint16_t    type_id;
    uint8_t     offset;
    int8_t      value_type;
    uint8_t     uom; // DeviceValueUOM
    std::string name;
    double      factor;
    bool        writeable;
    uint32_t    value;
    std::string data;
    uint8_t     ram;
    uint8_t *   raw;
};

class WebCustomEntity {
  public:
    std::list<CustomEntityItem> customEntityItems;

    static void              read(WebCustomEntity & webEntity, JsonObject root);
    static StateUpdateResult update(JsonObject root, WebCustomEntity & webEntity);
};

class WebCustomEntityService : public StatefulService<WebCustomEntity> {
  public:
    WebCustomEntityService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();
    void publish_single(CustomEntityItem & entity);
    void publish(const bool force = false);
    bool command_setvalue(const char * value, const int8_t id, const char * name);
    bool get_value_info(JsonObject output, const char * cmd);
    void get_value_json(JsonObject output, CustomEntityItem & entity);
    bool get_value(std::shared_ptr<const Telegram> telegram);
    void fetch();
    void render_value(JsonObject output, CustomEntityItem & entity, const bool useVal = false, const bool web = false, const bool add_uom = false);
    void show_values(JsonObject output);
    void generate_value_web(JsonObject output, const bool is_dashboard = false);

    uint8_t count_entities();
    void    ha_reset() {
        ha_registered_ = false;
    }

#if defined(EMSESP_TEST)
    void test();
#endif

  private:
    HttpEndpoint<WebCustomEntity>  _httpEndpoint;
    FSPersistence<WebCustomEntity> _fsPersistence;

    void getEntities(AsyncWebServerRequest * request);

    std::list<CustomEntityItem> * customEntityItems_; // pointer to the list of entity items

    bool ha_registered_ = false;
};

} // namespace emsesp

#endif