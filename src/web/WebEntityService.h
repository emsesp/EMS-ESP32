/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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
#include "../telegram.h"

#ifndef WebEntityService_h
#define WebEntityService_h

#define EMSESP_ENTITY_FILE "/config/emsespEntity.json"
#define EMSESP_ENTITY_SERVICE_PATH "/rest/entities" // GET and POST

namespace emsesp {

class EntityItem {
  public:
    uint8_t     id;
    uint8_t     device_id;
    uint16_t    type_id;
    uint8_t     offset;
    int8_t      value_type;
    uint8_t     uom;
    std::string name;
    double      factor;
    bool        writeable;
    uint32_t    value;
};

class WebEntity {
  public:
    std::list<EntityItem> entityItems;

    static void              read(WebEntity & webEntity, JsonObject & root);
    static StateUpdateResult update(JsonObject & root, WebEntity & webEntity);
};

class WebEntityService : public StatefulService<WebEntity> {
  public:
    WebEntityService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void    begin();
    void    publish_single(const EntityItem & entity);
    void    publish(const bool force = false);
    bool    command_setvalue(const char * value, const std::string name);
    bool    get_value_info(JsonObject & output, const char * cmd);
    bool    get_value(std::shared_ptr<const Telegram> telegram);
    void    fetch();
    void    render_value(JsonObject & output, EntityItem entity, const bool useVal = false, const bool web = false);
    uint8_t count_entities();
    void    generate_value_web(JsonObject & output);


  private:
    HttpEndpoint<WebEntity>  _httpEndpoint;
    FSPersistence<WebEntity> _fsPersistence;

    std::list<EntityItem> * entityItems; // pointer to the list of entity items
    bool                    ha_registered_ = false;
};

} // namespace emsesp

#endif