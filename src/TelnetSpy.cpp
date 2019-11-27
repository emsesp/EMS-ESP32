/*
 * TELNET SERVER FOR ESP8266 / ESP32
 * Cloning the serial port via Telnet.
 *
 * Written by Wolfgang Mattis (arduino@yasheena.de).
 * Version 1.1 / September 7, 2018.
 * MIT license, all text above must be included in any redistribution.
 */

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

#include "TelnetSpy.h"

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

static TelnetSpy * actualObject = NULL;

TelnetSpy::TelnetSpy() {
    port               = TELNETSPY_PORT;
    telnetServer       = NULL;
    started            = false;
    listening          = false;
    firstMainLoop      = true;
    usedSer            = &Serial;
    storeOffline       = true;
    connected          = false;
    callbackConnect    = NULL;
    callbackDisconnect = NULL;
    welcomeMsg         = strdup(TELNETSPY_WELCOME_MSG);
    rejectMsg          = strdup(TELNETSPY_REJECT_MSG);
    minBlockSize       = TELNETSPY_MIN_BLOCK_SIZE;
    collectingTime     = TELNETSPY_COLLECTING_TIME;
    maxBlockSize       = TELNETSPY_MAX_BLOCK_SIZE;
    pingTime           = TELNETSPY_PING_TIME;
    pingRef            = 0xFFFFFFFF;
    waitRef            = 0xFFFFFFFF;
    telnetBuf          = NULL;
    bufLen             = 0;
    uint16_t size      = TELNETSPY_BUFFER_LEN;
    while (!setBufferSize(size)) {
        size = size >> 1;
        if (size < minBlockSize) {
            setBufferSize(minBlockSize);
            break;
        }
    }
    debugOutput = TELNETSPY_CAPTURE_OS_PRINT;
    if (debugOutput) {
        setDebugOutput(true);
    }
}

TelnetSpy::~TelnetSpy() {
    end();
}

// added by proddy
void TelnetSpy::disconnectClient() {
    if (client.connected()) {
        client.flush();
        client.stop();
    }
    if (connected && (callbackDisconnect != NULL)) {
        callbackDisconnect();
    }
    connected = false;
}

void TelnetSpy::setPort(uint16_t portToUse) {
    port = portToUse;
    if (listening) {
        if (client.connected()) {
            client.flush();
            client.stop();
        }
        if (connected && (callbackDisconnect != NULL)) {
            callbackDisconnect();
        }
        connected = false;
        telnetServer->close();
        delete telnetServer;
        telnetServer = new WiFiServer(port);
        if (started) {
            telnetServer->begin();
            telnetServer->setNoDelay(bufLen > 0);
        }
    }
}

void TelnetSpy::setWelcomeMsg(const char * msg) {
    if (welcomeMsg) {
        free(welcomeMsg);
    }
    welcomeMsg = strdup(msg);
}

void TelnetSpy::setRejectMsg(const char * msg) {
    if (rejectMsg) {
        free(rejectMsg);
    }
    rejectMsg = strdup(msg);
}

void TelnetSpy::setMinBlockSize(uint16_t minSize) {
    minBlockSize = min(max((uint16_t)1, minSize), maxBlockSize);
}

void TelnetSpy::setCollectingTime(uint16_t colTime) {
    collectingTime = colTime;
}

void TelnetSpy::setMaxBlockSize(uint16_t maxSize) {
    maxBlockSize = max(maxSize, minBlockSize);
}

bool TelnetSpy::setBufferSize(uint16_t newSize) {
    if (telnetBuf && (bufLen == newSize)) {
        return true;
    }
    if (newSize == 0) {
        bufLen = 0;
        if (telnetBuf) {
            free(telnetBuf);
            telnetBuf = NULL;
        }
        if (telnetServer) {
            telnetServer->setNoDelay(false);
        }
        return true;
    }
    newSize            = max(newSize, minBlockSize);
    uint16_t oldBufLen = bufLen;
    bufLen             = newSize;
    uint16_t tmp;
    if (!telnetBuf || (bufUsed == 0)) {
        bufRdIdx = 0;
        bufWrIdx = 0;
        bufUsed  = 0;
    } else {
        if (bufLen < oldBufLen) {
            if (bufRdIdx < bufWrIdx) {
                if (bufWrIdx > bufLen) {
                    tmp = min(bufLen, (uint16_t)(bufWrIdx - max(bufLen, bufRdIdx)));
                    memcpy(telnetBuf, &telnetBuf[bufWrIdx - tmp], tmp);
                    bufWrIdx = tmp;
                    if (bufWrIdx > bufRdIdx) {
                        bufRdIdx = bufWrIdx;
                    } else {
                        if (bufRdIdx > bufLen) {
                            bufRdIdx = 0;
                        }
                    }
                    if (bufRdIdx == bufWrIdx) {
                        bufUsed = bufLen;
                    } else {
                        bufUsed = bufWrIdx - bufRdIdx;
                    }
                }
            } else {
                if (bufWrIdx > bufLen) {
                    memcpy(telnetBuf, &telnetBuf[bufWrIdx - bufLen], bufLen);
                    bufRdIdx = 0;
                    bufWrIdx = 0;
                    bufUsed  = bufLen;
                } else {
                    tmp = min(bufLen - bufWrIdx, oldBufLen - bufRdIdx);
                    memcpy(&telnetBuf[bufLen - tmp], &telnetBuf[oldBufLen - tmp], tmp);
                    bufRdIdx = bufLen - tmp;
                    bufUsed  = bufWrIdx + tmp;
                }
            }
        }
    }
    char * temp = (char *)realloc(telnetBuf, bufLen);
    if (!temp) {
        return false;
    }
    telnetBuf = temp;
    if (telnetBuf && (bufLen > oldBufLen) && (bufRdIdx > bufWrIdx)) {
        tmp = bufLen - (oldBufLen - bufRdIdx);
        memcpy(&telnetBuf[tmp], &telnetBuf[bufRdIdx], oldBufLen - bufRdIdx);
        bufRdIdx = tmp;
    }
    if (telnetServer) {
        telnetServer->setNoDelay(true);
    }
    return true;
}

uint16_t TelnetSpy::getBufferSize() {
    if (!telnetBuf) {
        return 0;
    }
    return bufLen;
}

void TelnetSpy::setStoreOffline(bool store) {
    storeOffline = store;
}

bool TelnetSpy::getStoreOffline() {
    return storeOffline;
}

void TelnetSpy::setPingTime(uint16_t pngTime) {
    pingTime = pngTime;
    if (pingTime == 0) {
        pingRef = 0xFFFFFFFF;
    } else {
        pingRef = (millis() & 0x7FFFFFF) + pingTime;
    }
}

void TelnetSpy::setSerial(HardwareSerial * usedSerial) {
    usedSer = usedSerial;
}

size_t TelnetSpy::write(uint8_t data) {
    if (telnetBuf) {
        if (storeOffline || client.connected()) {
            if (bufUsed == bufLen) {
                if (client.connected()) {
                    sendBlock();
                }
                if (bufUsed == bufLen) {
                    while (bufUsed > 0) {
                        char c = pullTelnetBuf();
                        if (c == '\n') {
                            addTelnetBuf('\r');
                            break;
                        }
                    }
                    if (peekTelnetBuf() == '\r') {
                        pullTelnetBuf();
                    }
                }
            }
            addTelnetBuf(data);
            /*
            if (data == '\n') {
                addTelnetBuf('\r'); // added by proddy, fix for Windows
            }
            */
        }
    } else {
        if (client.connected()) {
            client.write(data);
        }
    }
    if (usedSer) {
        return usedSer->write(data);
    }
    return 1;
}

// this still needs some work
bool TelnetSpy::isSerialAvailable(void) {
    if (usedSer) {
        usedSer->write("test");
        //Wait for four seconds or till data is available on serial, whichever occurs first.
        while (usedSer->available() == 0 && millis() < 4000)
            ;

        if (usedSer->available() > 0) {
            (void)usedSer->read(); // We then clear the input buffer
            return true;
        }
    }
    return false;
}

int TelnetSpy::available(void) {
    if (usedSer) {
        int avail = usedSer->available();
        if (avail > 0) {
            return avail;
        }
    }
    if (client.connected()) {
        return telnetAvailable();
    }
    return 0;
}

int TelnetSpy::read(void) {
    int val = 0;
    if (usedSer) {
        val = usedSer->read();
        if (val != -1) {
            return val;
        }
    }
    if (client.connected()) {
        if (telnetAvailable()) {
            val = client.read();
        }
    }
    return val;
}

int TelnetSpy::peek(void) {
    int val = 0;
    if (usedSer) {
        val = usedSer->peek();
        if (val != -1) {
            return val;
        }
    }
    if (client.connected()) {
        if (telnetAvailable()) {
            val = client.peek();
        }
    }
    return val;
}

void TelnetSpy::flush(void) {
    if (usedSer) {
        usedSer->flush();
    }
}

#ifdef ESP8266

void TelnetSpy::begin(unsigned long baud, SerialConfig config, SerialMode mode, uint8_t tx_pin) {
    if (usedSer) {
        usedSer->begin(baud, config, mode, tx_pin);
    }

    started = true;
}

#else // ESP32

void TelnetSpy::begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert) {
    if (usedSer) {
        usedSer->begin(baud, config, rxPin, txPin, invert);
    }
    started = true;
}

#endif

void TelnetSpy::end() {
    if (debugOutput) {
        setDebugOutput(false);
    }
    if (usedSer) {
        usedSer->end();
    }
    if (client.connected()) {
        client.flush();
        client.stop();
    }
    if (connected && (callbackDisconnect != NULL)) {
        callbackDisconnect();
    }
    connected = false;
    telnetServer->close();
    delete telnetServer;
    telnetServer = NULL;
    listening    = false;
    started      = false;
}

#ifdef ESP8266

void TelnetSpy::swap(uint8_t tx_pin) {
    if (usedSer) {
        usedSer->swap(tx_pin);
    }
}

void TelnetSpy::set_tx(uint8_t tx_pin) {
    if (usedSer) {
        usedSer->set_tx(tx_pin);
    }
}

void TelnetSpy::pins(uint8_t tx, uint8_t rx) {
    if (usedSer) {
        usedSer->pins(tx, rx);
    }
}

bool TelnetSpy::isTxEnabled(void) {
    if (usedSer) {
        return usedSer->isTxEnabled();
    }
    return true;
}

bool TelnetSpy::isRxEnabled(void) {
    if (usedSer) {
        return usedSer->isRxEnabled();
    }
    return true;
}

#endif

int TelnetSpy::availableForWrite(void) {
    if (usedSer) {
        return min(usedSer->availableForWrite(), bufLen - bufUsed);
    }
    return bufLen - bufUsed;
}

TelnetSpy::operator bool() const {
    if (usedSer) {
        return (bool)*usedSer;
    }
    return true;
}

void TelnetSpy::setDebugOutput(bool en) {
    debugOutput = en;

    if (debugOutput) {
        actualObject = this;
    } else {
        if (actualObject == this) {
            actualObject = NULL;
        }
    }
}

uint32_t TelnetSpy::baudRate(void) {
    if (usedSer) {
        return usedSer->baudRate();
    }
    return 115200;
}

void TelnetSpy::sendBlock() {
    uint16_t len = bufUsed;
    if (len > maxBlockSize) {
        len = maxBlockSize;
    }
    len = min(len, (uint16_t)(bufLen - bufRdIdx));
    client.write(&telnetBuf[bufRdIdx], len);
    bufRdIdx += len;
    if (bufRdIdx >= bufLen) {
        bufRdIdx = 0;
    }
    bufUsed -= len;
    if (bufUsed == 0) {
        bufRdIdx = 0;
        bufWrIdx = 0;
    }
    waitRef = 0xFFFFFFFF;
    if (pingRef != 0xFFFFFFFF) {
        pingRef = (millis() & 0x7FFFFFF) + pingTime;
        if (pingRef > 0x7FFFFFFF) {
            pingRef -= 0x80000000;
        }
    }
}

void TelnetSpy::addTelnetBuf(char c) {
    telnetBuf[bufWrIdx] = c;
    if (bufUsed == bufLen) {
        bufRdIdx++;
        if (bufRdIdx >= bufLen) {
            bufRdIdx = 0;
        }
    } else {
        bufUsed++;
    }
    bufWrIdx++;
    if (bufWrIdx >= bufLen) {
        bufWrIdx = 0;
    }
}

char TelnetSpy::pullTelnetBuf() {
    if (bufUsed == 0) {
        return 0;
    }
    char c = telnetBuf[bufRdIdx++];
    if (bufRdIdx >= bufLen) {
        bufRdIdx = 0;
    }
    bufUsed--;
    return c;
}

char TelnetSpy::peekTelnetBuf() {
    if (bufUsed == 0) {
        return 0;
    }
    return telnetBuf[bufRdIdx];
}

int TelnetSpy::telnetAvailable() {
    int n = client.available();
    while (n > 0) {
        if (0xff == client.peek()) { // If esc char for telnet NVT protocol data remove that telegram:
            client.read();           // Remove esc char
            n--;
            if (0xff == client.peek()) { // If esc sequence for 0xFF data byte...
                return n;                // ...return info about available data (just this 0xFF data byte)
            }
            client.read(); // Skip the rest of the telegram of the telnet NVT protocol data
            client.read();
            n--;
            n--;
        } else {      // If next char is a normal data byte...
            return n; // ...return info about available data
        }
    }
    return 0;
}

bool TelnetSpy::isClientConnected() {
    return connected;
}

void TelnetSpy::setCallbackOnConnect(telnetSpyCallback callback) {
    callbackConnect = callback;
}

void TelnetSpy::setCallbackOnDisconnect(telnetSpyCallback callback) {
    callbackDisconnect = callback;
}

void TelnetSpy::serialPrint(char c) {
    if (usedSer) {
        usedSer->print(c);
    }
}

void TelnetSpy::handle() {
    if (firstMainLoop) {
        firstMainLoop = false;
        // Between setup() and loop() the configuration for os_print may be changed so it must be renewed
        if (debugOutput && (actualObject == this)) {
            setDebugOutput(true);
        }
    }
    if (!started) {
        return;
    }
    if (!listening) {
        if ((WiFi.status() == WL_DISCONNECTED) && (WiFi.getMode() & WIFI_AP)) {
            if (usedSer) {
                usedSer->println("[TELNET] in AP mode"); // added by Proddy
            }
        } else if (WiFi.status() != WL_CONNECTED) {
            return;
        }

        telnetServer = new WiFiServer(port);
        telnetServer->begin();
        telnetServer->setNoDelay(bufLen > 0);
        listening = true;
        if (usedSer) {
            usedSer->println("[TELNET] Telnet server started"); // added by Proddy
        }
    }
    if (telnetServer->hasClient()) {
        if (client.connected()) {
            WiFiClient rejectClient = telnetServer->available();
            if (strlen(rejectMsg) > 0) {
                rejectClient.write((const uint8_t *)rejectMsg, strlen(rejectMsg));
            }
            rejectClient.flush();
            rejectClient.stop();
        } else {
            client = telnetServer->available();
            if (strlen(welcomeMsg) > 0) {
                client.write((const uint8_t *)welcomeMsg, strlen(welcomeMsg));
            }
        }
    }
    if (client.connected()) {
        if (!connected) {
            connected = true;
            if (pingTime != 0) {
                pingRef = (millis() & 0x7FFFFFF) + pingTime;
            }
            if (callbackConnect != NULL) {
                callbackConnect();
            }
        }
    } else {
        if (connected) {
            connected = false;
            client.flush();
            client.stop();
            pingRef = 0xFFFFFFFF;
            waitRef = 0xFFFFFFFF;
            if (callbackDisconnect != NULL) {
                callbackDisconnect();
            }
        }
    }

    if (client.connected() && (bufUsed > 0)) {
        if (bufUsed >= minBlockSize) {
            sendBlock();
        } else {
            unsigned long m = millis() & 0x7FFFFFF;
            if (waitRef == 0xFFFFFFFF) {
                waitRef = m + collectingTime;
                if (waitRef > 0x7FFFFFFF) {
                    waitRef -= 0x80000000;
                }
            } else {
                if (!((waitRef < 0x20000000) && (m > 0x60000000)) && (m >= waitRef)) {
                    sendBlock();
                }
            }
        }
    }
    if (client.connected() && (pingRef != 0xFFFFFFFF)) {
        unsigned long m = millis() & 0x7FFFFFF;
        if (!((pingRef < 0x20000000) && (m > 0x60000000)) && (m >= pingRef)) {
            addTelnetBuf(0);
            sendBlock();
        }
    }
}
