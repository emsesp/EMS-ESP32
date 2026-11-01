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

#include "emsesp.h"

namespace emsesp {

namespace {

// collects what the shell prints, up to a limit. There is never any input
// terminal escape sequences and control characters other than \n and \t are dropped,
// ArduinoJson doesn't escape them so they would make the JSON response invalid
class CaptureStream : public ::Stream {
  public:
    explicit CaptureStream(size_t limit)
        : limit_(limit) {
    }

    int available() override {
        return 0;
    }
    int read() override {
        return -1;
    }
    int peek() override {
        return -1;
    }
    size_t write(uint8_t data) override {
        return write(&data, 1);
    }
    size_t write(const uint8_t * buffer, size_t size) override {
        for (size_t i = 0; i < size; i++) {
            const uint8_t c = buffer[i];
            if (escape_ == Escape::START) {
                escape_ = (c == '[') ? Escape::CSI : Escape::NONE;
                continue;
            }
            if (escape_ == Escape::CSI) {
                if (c >= 0x40 && c <= 0x7E) {
                    escape_ = Escape::NONE;
                }
                continue;
            }
            if (c == 0x1B) {
                escape_ = Escape::START;
                continue;
            }
            if ((c < 0x20 && c != '\n' && c != '\t') || c == 0x7F) {
                continue;
            }
            if (output_.size() >= limit_) {
                truncated_ = true;
                break;
            }
            output_.push_back(static_cast<char>(c));
        }
        return size;
    }
    void flush() override {
    }

    const stringPSRAM & output() const {
        return output_;
    }
    bool truncated() const {
        return truncated_;
    }

  private:
    enum class Escape : uint8_t { NONE, START, CSI };

    size_t      limit_;
    stringPSRAM output_;
    bool        truncated_ = false;
    Escape      escape_    = Escape::NONE;
};

} // namespace

WebConsoleService::WebConsoleService(AsyncWebServer * server, SecurityManager * securityManager)
    : securityManager_(securityManager) {
    securityManager->addEndpoint(server,
                                 EMSESP_CONSOLE_PATH,
                                 AuthenticationPredicates::IS_ADMIN,
                                 [this](AsyncWebServerRequest * request, JsonVariant json) { runCommand(request, json); });
}

void WebConsoleService::begin(EMSESP & emsesp) {
    emsesp_ = &emsesp;
}

// POST {"command": "show values"} returns {"output": "..."}
void WebConsoleService::runCommand(AsyncWebServerRequest * request, JsonVariant json) {
    const char * command = json["command"];
    if (emsesp_ == nullptr || command == nullptr || strlen(command) > MAX_COMMAND_LENGTH) {
        request->send(400);
        return;
    }

    // only one command at a time, the shell output isn't shared between requests
    static bool busy = false;
    if (busy) {
        request->send(429);
        return;
    }
    busy = true;

    Authentication authentication = securityManager_->authenticateRequest(request);
    EMSESP::logger().notice("Web console command '%s' from user '%s'", command, authentication.user ? authentication.user->username.c_str() : "?");

    CaptureStream stream(EMSESP::system_.PSram() ? MAX_OUTPUT_PSRAM : MAX_OUTPUT);
    std::make_shared<EMSESPWebShell>(*emsesp_, stream)->run(command);

    auto *     response = new PsramAsyncJsonResponse();
    JsonObject root     = response->getRoot();
    root["output"]      = stream.output().c_str();
    if (stream.truncated()) {
        root["truncated"] = true;
    }
    response->setLength();
    request->send(response);

    busy = false;
}

} // namespace emsesp
