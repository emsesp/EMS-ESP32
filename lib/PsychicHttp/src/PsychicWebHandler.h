#ifndef PsychicWebHandler_h
#define PsychicWebHandler_h

// #include "PsychicCore.h"
// #include "PsychicHttpServer.h"
// #include "PsychicRequest.h"
#include "PsychicHandler.h"

/*
* HANDLER :: Can be attached to any endpoint or as a generic request handler.
*/

class PsychicWebHandler : public PsychicHandler {
  protected:
    PsychicHttpRequestCallback _requestCallback;
    PsychicClientCallback _onOpen;
    PsychicClientCallback _onClose;

  public:
    PsychicWebHandler();
    ~PsychicWebHandler();

    virtual bool canHandle(PsychicRequest *request) override;
    virtual esp_err_t handleRequest(PsychicRequest *request) override;
    PsychicWebHandler * onRequest(PsychicHttpRequestCallback fn);

    virtual void openCallback(PsychicClient *client);
    virtual void closeCallback(PsychicClient *client);

    PsychicWebHandler *onOpen(PsychicClientCallback fn);
    PsychicWebHandler *onClose(PsychicClientCallback fn);
};

#endif