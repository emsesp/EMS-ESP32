#ifndef ChunkPrinter_h
#define ChunkPrinter_h

// #include "PsychicCore.h"
#include "PsychicResponse.h"
#include <Print.h>

class ChunkPrinter : public Print
{
  private:
    PsychicResponse *_response;
    uint8_t *_buffer;
    size_t _length;
    size_t _pos;

  public:
    ChunkPrinter(PsychicResponse *response, uint8_t *buffer, size_t len) :
      _response(response),
      _buffer(buffer),
      _length(len),
      _pos(0)
    {}

    virtual ~ChunkPrinter() {}

    size_t write(uint8_t c)
    {
      esp_err_t err;

      _buffer[_pos] = c;
      _pos++;

      //if we're full, send a chunk
      if (_pos == _length)
      {
        _pos = 0;

        err = _response->sendChunk(_buffer, _length);
        if (err != ESP_OK)
          return 0;
      }

      return 1;
    }

    virtual void flush() override
    {
      if (_pos)
      {
        _response->sendChunk(_buffer, _pos);
        _pos = 0;
      }
    }
};

#endif