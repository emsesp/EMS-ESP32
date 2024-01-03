#ifndef PsychicStreamResponse_h
#define PsychicStreamResponse_h

#include "PsychicCore.h"
#include "PsychicResponse.h"
#include "ChunkPrinter.h"

class PsychicRequest;

class PsychicStreamResponse : public PsychicResponse, public Print
{
  private:
    ChunkPrinter *_printer;
    uint8_t *_buffer;
  public:
  
    PsychicStreamResponse(PsychicRequest *request, const String& contentType);
    PsychicStreamResponse(PsychicRequest *request, const String& contentType, const String& name); //Download
  
    ~PsychicStreamResponse();
  
    esp_err_t beginSend();
    esp_err_t endSend();
  
    virtual void flush() override;

    size_t write(uint8_t data);
    size_t copyFrom(Stream &stream);
  
    using Print::write;
};

#endif // PsychicStreamResponse_h
