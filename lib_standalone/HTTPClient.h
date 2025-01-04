#ifndef HTTPClient_H_
#define HTTPClient_H_

#include "WString.h"

class HTTPClient {
  public:
    bool begin(String url) {
        return true;
    };
    void end(void) {};
    int  GET() {
        return 200;
    };
    int POST(String payload) {
        return 200;
    };
    void   addHeader(const String & name, const String & value, bool first = false, bool replace = true) {};
    String getString(void) {
        return "Hello, World!";
    };
};

#endif /* HTTPClient_H_ */
