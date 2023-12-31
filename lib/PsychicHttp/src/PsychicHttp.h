#ifndef PsychicHttp_h
#define PsychicHttp_h

//#define ENABLE_ASYNC // This is something added in ESP-IDF 5.1.x where each request can be handled in its own thread

#include <http_status.h>
#include "PsychicHttpServer.h"
#include "PsychicRequest.h"
#include "PsychicResponse.h"
#include "PsychicEndpoint.h"
#include "PsychicHandler.h"
#include "PsychicStaticFileHandler.h"
#include "PsychicFileResponse.h"
#include "PsychicUploadHandler.h"
#include "PsychicWebSocket.h"
#include "PsychicEventSource.h"
#include "PsychicJson.h"

#ifdef ENABLE_ASYNC
#include "async_worker.h"
#endif

#endif /* PsychicHttp_h */