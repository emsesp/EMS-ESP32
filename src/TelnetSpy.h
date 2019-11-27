/*
 * TELNET SERVER FOR ESP8266 / ESP32
 * Cloning the serial port via Telnet.
 *
 * Written by Wolfgang Mattis (arduino@yasheena.de).
 * Version 1.1 / September 7, 2018.
 * MIT license, all text above must be included in any redistribution.
 */

/*
 * DESCRIPTION
 *
 * This module allows you "Debugging over the air". So if you already use
 * ArduinoOTA this is a helpful extension for wireless development. Use
 * "TelnetSpy" instead of "Serial" to send data to the serial port and a copy
 * to a telnet connection. There is a circular buffer which allows to store the
 * data while the telnet connection is not established. So its possible to
 * collect data even when the Wifi and Telnet connections are still not
 * established. Its also possible to create a telnet session only if it is
 * neccessary: then you will get the already collected data as far as it is
 * still stored in the circular buffer. Data send from telnet terminal to
 * ESP8266 / ESP32 will be handled as data received by serial port. It is also
 * possible to use more than one instance of TelnetSpy, for example to send
 * control information on the first instance and data dumps on the second
 * instance.
 *
 * USAGE
 *
 * Add the following line to your sketch:
 *		#include <TelnetSpy.h>
 *		TelnetSpy LOG;
 *
 * Add the following line to your initialisation block ( void setup() ):
 *		LOG.begin();
 *
 * Add the following line at the beginning of your main loop ( void loop() ):
 *		LOG.handle();
 *
 * Use the following functions of the TelnetSpy object to modify behavior
 *
 * Change the port number of this telnet server. If a client is already
 * connected it will be disconnected.
 * Default: 23
 *		void setPort(uint16_t portToUse);
 *
 * Change the message which will be send to the telnet client after a session
 * is established.
 * Default: "Connection established via TelnetSpy.\n"
 *		void setWelcomeMsg(char* msg);
 *
 * Change the message which will be send to the telnet client if another
 * session is already established.
 * Default: "TelnetSpy: Only one connection possible.\n"
 *		void setRejectMsg(char* msg);
 *
 * Change the amount of characters to collect before sending a telnet block.
 * Default: 64
 *		void setMinBlockSize(uint16_t minSize);
 *
 * Change the time (in ms) to wait before sending a telnet block if its size is
 * less than <minSize> (defined by setMinBlockSize).
 * Default: 100
 *		void setCollectingTime(uint16_t colTime);
 *
 * Change the maximum size of the telnet packets to send.
 * Default: 512
 *		void setMaxBlockSize(uint16_t maxSize);
 *
 * Change the size of the ring buffer. Set it to 0 to disable buffering.
 * Changing size tries to preserve the already collected data. If the new
 * buffer size is too small the youngest data will be preserved only. Returns
 * false if the requested buffer size cannot be set.
 * Default: 3000
 *		bool setBufferSize(uint16_t newSize);
 *
 * This function returns the actual size of the ring buffer.
 *		uint16_t getBufferSize();
 *
 * Enable / disable storing new data in the ring buffer if no telnet connection
 * is established. This function allows you to store important data only. You
 * can do this by disabling "storeOffline" for sending less important data.
 * Default: true
 *		void setStoreOffline(bool store);
 *
 * Get actual state of storing data when offline.
 *		bool getStoreOffline();
 *
 * If no data is sent via TelnetSpy the detection of a disconnected client has
 * a long timeout. Use setPingTime to define the time (in ms) without traffic
 * after which a ping (chr(0)) is sent to the telnet client to detect a
 * disconnect earlier. Use 0 as parameter to disable pings.
 * Default: 1500
 *		void setPingTime(uint16_t pngTime);
 *
 * Set the serial port you want to use with this object (especially for ESP32)
 * or NULL if no serial port should be used (telnet only).
 * Default: Serial
 *		void setSerial(HardwareSerial* usedSerial);
 *
 * This function returns true, if a telnet client is connected.
 *		bool isClientConnected();
 *
 * This function installs a callback function which will be called on every
 * telnet connect of this object (except rejected connect tries). Use NULL to
 * remove the callback.
 * Default: NULL
 *		void setCallbackOnConnect(void (*callback)());
 *
 * This function installs a callback function which will be called on every
 * telnet disconnect of this object (except rejected connect tries). Use NULL
 * to remove the callback.
 * Default: NULL
 *		void setCallbackOnDisconnect(void (*callback)());
 *
 * HINT
 *
 * Add the following lines to your sketch:
 *		#define SERIAL TelnetSpy
 *		//#define SERIAL Serial
 *
 * Replace "Serial" with "SERIAL" in your sketch. Now you can switch between
 * serial only and serial with telnet by changing the comments of the defines
 * only.
 *
 * IMPORTANT
 *
 * To connect to the telnet server you have to:
 *	- establish the Wifi connection
 *	- execute "TelnetSpy.begin(WhatEverYouWant);"
 *
 * The order is not important.
 *
 * All you do with "Serial" you can also do with "TelnetSpy", but remember:
 * Transfering data also via telnet will need more performance than the serial
 * port only. So time critical things may be influenced.
 *
 * It is not possible to establish more than one telnet connection at the same
 * time. But its possible to use more than one instance of TelnetSpy.
 *
 * If you have problems with low memory you may reduce the value of the define
 * TELNETSPY_BUFFER_LEN for a smaller ring buffer on initialisation.
 *
 * Usage of void setDebugOutput(bool) to enable / disable of capturing of
 * os_print calls when you have more than one TelnetSpy instance: That
 * TelnetSpy object will handle this functionality where you used
 * setDebugOutput at last. On default TelnetSpy has the capturing of OS_print
 * calls enabled. So if you have more instances the last created instance will
 * handle the capturing.
 */

#ifndef TelnetSpy_h
#define TelnetSpy_h

#define TELNETSPY_BUFFER_LEN 1000 // was 3000
#define TELNETSPY_MIN_BLOCK_SIZE 64
#define TELNETSPY_COLLECTING_TIME 100
#define TELNETSPY_MAX_BLOCK_SIZE 512
#define TELNETSPY_PING_TIME 1500
#define TELNETSPY_PORT 23
#define TELNETSPY_CAPTURE_OS_PRINT false
#define TELNETSPY_WELCOME_MSG "Connection established via Telnet.\n"
#define TELNETSPY_REJECT_MSG "Telnet: Only one connection possible.\n"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else // ESP32
#include <WiFi.h>
#endif
#include <WiFiClient.h>

class TelnetSpy : public Stream {
  public:
    TelnetSpy();
    ~TelnetSpy();
    void     handle(void);
    void     setPort(uint16_t portToUse);
    void     setWelcomeMsg(const char * msg);
    void     setRejectMsg(const char * msg);
    void     setMinBlockSize(uint16_t minSize);
    void     setCollectingTime(uint16_t colTime);
    void     setMaxBlockSize(uint16_t maxSize);
    bool     setBufferSize(uint16_t newSize);
    uint16_t getBufferSize();
    void     setStoreOffline(bool store);
    bool     getStoreOffline();
    void     setPingTime(uint16_t pngTime);
    void     setSerial(HardwareSerial * usedSerial);
    bool     isClientConnected();
    void     serialPrint(char c);

    void                          disconnectClient();                                  // added by Proddy
    typedef std::function<void()> telnetSpyCallback;                                   // added by Proddy
    void                          setCallbackOnConnect(telnetSpyCallback callback);    // changed by proddy
    void                          setCallbackOnDisconnect(telnetSpyCallback callback); // changed by proddy

    // Functions offered by HardwareSerial class:
#ifdef ESP8266
    void begin(unsigned long baud) {
        begin(baud, SERIAL_8N1, SERIAL_FULL, 1);
    }
    void begin(unsigned long baud, SerialConfig config) {
        begin(baud, config, SERIAL_FULL, 1);
    }
    void begin(unsigned long baud, SerialConfig config, SerialMode mode) {
        begin(baud, config, mode, 1);
    }
    void begin(unsigned long baud, SerialConfig config, SerialMode mode, uint8_t tx_pin);
#else // ESP32
    void begin(unsigned long baud, uint32_t config = SERIAL_8N1, int8_t rxPin = -1, int8_t txPin = -1, bool invert = false);
#endif
    void end();
#ifdef ESP8266
    void swap() {
        swap(1);
    }
    void swap(uint8_t tx_pin);
    void set_tx(uint8_t tx_pin);
    void pins(uint8_t tx, uint8_t rx);
    bool isTxEnabled(void);
    bool isRxEnabled(void);
#endif
    int           available(void) override;
    int           peek(void) override;
    int           read(void) override;
    int           availableForWrite(void);
    void          flush(void) override;
    size_t        write(uint8_t) override;
    inline size_t write(unsigned long n) {
        return write((uint8_t)n);
    }
    inline size_t write(long n) {
        return write((uint8_t)n);
    }
    inline size_t write(unsigned int n) {
        return write((uint8_t)n);
    }
    inline size_t write(int n) {
        return write((uint8_t)n);
    }
    using Print::write;
             operator bool() const;
    void     setDebugOutput(bool);
    uint32_t baudRate(void);

    bool isSerialAvailable(void);

  protected:
    void             sendBlock(void);
    void             addTelnetBuf(char c);
    char             pullTelnetBuf();
    char             peekTelnetBuf();
    int              telnetAvailable();
    WiFiServer *     telnetServer;
    WiFiClient       client;
    uint16_t         port;
    HardwareSerial * usedSer;
    bool             storeOffline;
    bool             started;
    bool             listening;
    bool             firstMainLoop;
    unsigned long    waitRef;
    unsigned long    pingRef;
    uint16_t         pingTime;
    char *           welcomeMsg;
    char *           rejectMsg;
    uint16_t         minBlockSize;
    uint16_t         collectingTime;
    uint16_t         maxBlockSize;
    bool             debugOutput;
    char *           telnetBuf;
    uint16_t         bufLen;
    uint16_t         bufUsed;
    uint16_t         bufRdIdx;
    uint16_t         bufWrIdx;
    bool             connected;

    telnetSpyCallback callbackConnect;    // added by proddy
    telnetSpyCallback callbackDisconnect; // added by proddy
};

#endif
