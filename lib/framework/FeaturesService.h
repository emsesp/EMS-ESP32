#ifndef FeaturesService_h
#define FeaturesService_h

#include <Features.h>

#include <WiFi.h>
#include <ArduinoJson.h>
#include <PsychicHttp.h>

#define FEATURES_SERVICE_PATH "/rest/features"

class FeaturesService {
  public:
    FeaturesService(PsychicHttpServer * server);

    void registerURI();

  private:
    PsychicHttpServer * _server;
};

#endif
