
/*
 * ems.cpp
 * handles all the EMS messages
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP-Boiler
 */

#include "ems.h"
#include "emsuart.h"

#include <Arduino.h>
#include <ESPHelper.h>
#include <TimeLib.h>

_EMS_Sys_Status EMS_Sys_Status; // EMS Status
_EMS_TxTelegram EMS_TxTelegram; // Empty buffer for sending telegrams

// call back for handling Types
#define MAX_TYPECALLBACK 11
const _EMS_Types EMS_Types[MAX_TYPECALLBACK] =
{   {EMS_ID_BOILER, EMS_TYPE_UBAMonitorFast, "UBAMonitorFast", 36, _process_UBAMonitorFast},
    {EMS_ID_BOILER, EMS_TYPE_UBAMonitorSlow, "UBAMonitorSlow", 28, _process_UBAMonitorSlow},
    {EMS_ID_BOILER, EMS_TYPE_UBAMonitorWWMessage, "UBAMonitorWWMessage", 10, _process_UBAMonitorWWMessage},
    {EMS_ID_BOILER, EMS_TYPE_UBAParameterWW, "UBAParameterWW", 10, _process_UBAParameterWW},
    {EMS_ID_BOILER, EMS_TYPE_UBATotalUptimeMessage, "UBATotalUptimeMessage", 30, NULL},
    {EMS_ID_BOILER, EMS_TYPE_UBAMaintenanceSettingsMessage, "UBAMaintenanceSettingsMessage", 30, NULL},
    {EMS_ID_BOILER, EMS_TYPE_UBAParametersMessage, "UBAParametersMessage", 30, NULL},
    {EMS_ID_BOILER, EMS_TYPE_UBAMaintenanceStatusMessage, "UBAMaintenanceStatusMessage", 30, NULL},

    {EMS_ID_THERMOSTAT, EMS_TYPE_RC20StatusMessage, "RC20StatusMessage", 3, _process_RC20StatusMessage},
    {EMS_ID_THERMOSTAT, EMS_TYPE_RC20Time, "RC20Time", 20, _process_RC20Time},
    {EMS_ID_THERMOSTAT, EMS_TYPE_RC20Temperature, "RC20Temperature", 10, _process_RC20Temperature}
};


// reserve space for the data we collect from the Boiler and Thermostat
_EMS_Boiler     EMS_Boiler;
_EMS_Thermostat EMS_Thermostat;

// CRC lookup table with poly 12 for faster checking
const uint8_t ems_crc_table[] =
{   0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24,
    0x26, 0x28, 0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E, 0x40, 0x42, 0x44, 0x46, 0x48, 0x4A,
    0x4C, 0x4E, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C, 0x6E, 0x70,
    0x72, 0x74, 0x76, 0x78, 0x7A, 0x7C, 0x7E, 0x80, 0x82, 0x84, 0x86, 0x88, 0x8A, 0x8C, 0x8E, 0x90, 0x92, 0x94, 0x96,
    0x98, 0x9A, 0x9C, 0x9E, 0xA0, 0xA2, 0xA4, 0xA6, 0xA8, 0xAA, 0xAC, 0xAE, 0xB0, 0xB2, 0xB4, 0xB6, 0xB8, 0xBA, 0xBC,
    0xBE, 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE, 0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE, 0xE0, 0xE2,
    0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE, 0xF0, 0xF2, 0xF4, 0xF6, 0xF8, 0xFA, 0xFC, 0xFE, 0x19, 0x1B, 0x1D, 0x1F, 0x11,
    0x13, 0x15, 0x17, 0x09, 0x0B, 0x0D, 0x0F, 0x01, 0x03, 0x05, 0x07, 0x39, 0x3B, 0x3D, 0x3F, 0x31, 0x33, 0x35, 0x37,
    0x29, 0x2B, 0x2D, 0x2F, 0x21, 0x23, 0x25, 0x27, 0x59, 0x5B, 0x5D, 0x5F, 0x51, 0x53, 0x55, 0x57, 0x49, 0x4B, 0x4D,
    0x4F, 0x41, 0x43, 0x45, 0x47, 0x79, 0x7B, 0x7D, 0x7F, 0x71, 0x73, 0x75, 0x77, 0x69, 0x6B, 0x6D, 0x6F, 0x61, 0x63,
    0x65, 0x67, 0x99, 0x9B, 0x9D, 0x9F, 0x91, 0x93, 0x95, 0x97, 0x89, 0x8B, 0x8D, 0x8F, 0x81, 0x83, 0x85, 0x87, 0xB9,
    0xBB, 0xBD, 0xBF, 0xB1, 0xB3, 0xB5, 0xB7, 0xA9, 0xAB, 0xAD, 0xAF, 0xA1, 0xA3, 0xA5, 0xA7, 0xD9, 0xDB, 0xDD, 0xDF,
    0xD1, 0xD3, 0xD5, 0xD7, 0xC9, 0xCB, 0xCD, 0xCF, 0xC1, 0xC3, 0xC5, 0xC7, 0xF9, 0xFB, 0xFD, 0xFF, 0xF1, 0xF3, 0xF5,
    0xF7, 0xE9, 0xEB, 0xED, 0xEF, 0xE1, 0xE3, 0xE5, 0xE7
};

extern ESPHelper myESP;

#define myDebug(x, ...) myESP.printf(x, ##__VA_ARGS__);

// constants timers
const uint64_t RX_READ_TIMEOUT       = 5000; // in ms. 5 seconds timeout for read replies
const uint8_t  RX_READ_TIMEOUT_COUNT = 4;    // 4 retries before timeout

uint8_t emsLastRxCount = 0;

// init stats and counters and buffers
void ems_init() {
    // overall status
    EMS_Sys_Status.emsRxPgks    = 0;
    EMS_Sys_Status.emsTxPkgs    = 0;
    EMS_Sys_Status.emxCrcErr    = 0;
    EMS_Sys_Status.emsRxStatus  = EMS_RX_IDLE;
    EMS_Sys_Status.emsTxStatus  = EMS_TX_IDLE;
    EMS_Sys_Status.emsLastPoll  = 0;
    EMS_Sys_Status.emsLastRx    = 0;
    EMS_Sys_Status.emsLastTx    = 0;
    EMS_Sys_Status.emsRefreshed = false;

    EMS_Sys_Status.emsPollEnabled       = false; // start up with Poll disabled
    EMS_Sys_Status.emsThermostatEnabled = true;  // there is a RCxx thermostat active
    EMS_Sys_Status.emsLogVerbose        = false; // Verbose logging is off

    EMS_Thermostat.hour   = 0;
    EMS_Thermostat.minute = 0;
    EMS_Thermostat.second = 0;
    EMS_Thermostat.day    = 0;
    EMS_Thermostat.month  = 0;
    EMS_Thermostat.year   = 0;

    EMS_Boiler.wWActivated   = false; // Warm Water activated
    EMS_Boiler.wWSelTemp     = 0;     // Warm Water selected temperature
    EMS_Boiler.wWCircPump    = false; // Warm Water circulation pump Available
    EMS_Boiler.wWDesiredTemp = 0;     // Warm Water desired temperature

    // UBAMonitorFast
    EMS_Boiler.selFlowTemp = 0;     // Selected flow temperature
    EMS_Boiler.curFlowTemp = -1;    // Current flow temperature
    EMS_Boiler.retTemp     = -1;    // Return temperature
    EMS_Boiler.burnGas     = false; // Gas on/off
    EMS_Boiler.fanWork     = false; // Fan on/off
    EMS_Boiler.ignWork     = false; // Ignition on/off
    EMS_Boiler.heatPmp     = false; // Circulating pump on/off
    EMS_Boiler.wWHeat      = false; // 3-way valve on WW
    EMS_Boiler.wWCirc      = false; // Circulation on/off
    EMS_Boiler.selBurnPow  = 0;     // Burner max power
    EMS_Boiler.curBurnPow  = 0;     // Burner current power
    EMS_Boiler.flameCurr   = -1;    // Flame current in micro amps
    EMS_Boiler.sysPress    = -1;    // System pressure

    // UBAMonitorSlow
    EMS_Boiler.extTemp     = -1; // Outside temperature
    EMS_Boiler.boilTemp    = -1; // Boiler temperature
    EMS_Boiler.pumpMod     = 0;  // Pump modulation
    EMS_Boiler.burnStarts  = 0;  // # burner restarts
    EMS_Boiler.burnWorkMin = 0;  // Total burner operating time
    EMS_Boiler.heatWorkMin = 0;  // Total heat operating time

    // UBAMonitorWWMessage
    EMS_Boiler.wWCurTmp  = -1;    // Warm Water current temperature:
    EMS_Boiler.wWStarts  = 0;     // Warm Water # starts
    EMS_Boiler.wWWorkM   = 0;     // Warm Water # minutes
    EMS_Boiler.wWOneTime = false; // Warm Water one time function on/off

    // init the Tx package
    _initTxBuffer();
}

// init Tx Buffer
void _initTxBuffer() {
    EMS_TxTelegram.length        = 0;
    EMS_TxTelegram.type          = 0;
    EMS_TxTelegram.dest          = 0;
    EMS_TxTelegram.offset        = 0;
    EMS_TxTelegram.length        = 0;
    EMS_TxTelegram.type_validate = 0;
    EMS_TxTelegram.action        = EMS_TX_NONE;
    EMS_Sys_Status.emsTxStatus   = EMS_TX_IDLE;
    emsLastRxCount               = 0;
}

// Getters and Setters for parameters
void ems_setPoll(bool b) {
    EMS_Sys_Status.emsPollEnabled = b;
    myDebug("EMS Bus Poll is %s\n", EMS_Sys_Status.emsPollEnabled ? "enabled" : "disabled");
}

bool ems_getPoll() {
    return EMS_Sys_Status.emsPollEnabled;
}

bool ems_getThermostatEnabled() {
    return EMS_Sys_Status.emsThermostatEnabled;
}

void ems_setThermostatEnabled(bool b) {
    EMS_Sys_Status.emsThermostatEnabled = b;
    myDebug("Thermostat is %s\n", EMS_Sys_Status.emsThermostatEnabled ? "enabled" : "disabled");
}

bool ems_getLogVerbose() {
    return EMS_Sys_Status.emsLogVerbose;
}

void ems_setLogVerbose(bool b) {
    EMS_Sys_Status.emsLogVerbose = b;
    myDebug("Verbose logging is %s.\n", EMS_Sys_Status.emsLogVerbose ? "on" : "off");
}

/*
 * Calculate CRC checksum using lookup table
 * len is length of data in bytes (including the CRC byte at end)
 */
uint8_t _crcCalculator(uint8_t * data, uint8_t len) {
    uint8_t crc = 0;

    // read data and stop before the CRC
    for (uint8_t i = 0; i < len - 1; i++) {
        crc = ems_crc_table[crc];
        crc ^= data[i];
    }

    return crc;
}

// debug print a telegram to telnet console
// len is length in bytes including the CRC
void _debugPrintTelegram(const char * prefix, uint8_t * data, uint8_t len, const char * color) {
    if (!EMS_Sys_Status.emsLogVerbose)
        return;

    bool crcok = (data[len - 1] == _crcCalculator(data, len));
    if (crcok) {
        myDebug(color)
    } else {
        myDebug(COLOR_RED);
    }

    time_t currentTime = now();
    myDebug("[%02d:%02d:%02d] %s len=%02d, data: ", hour(currentTime), minute(currentTime), second(currentTime), prefix, len);
    for (int i = 0; i < len; i++) {
        myDebug("%02x ", data[i]);
    }
    myDebug("(%s) %s\n", crcok ? "OK" : "BAD", COLOR_RESET);
}

// send the contents of the Tx buffer
void _ems_sendTelegram() {
    // only send when Tx is not busy
    _debugPrintTelegram(((EMS_TxTelegram.action == EMS_TX_WRITE) ? "Sending write telegram:" : "Sending read telegram:"),
                        EMS_TxTelegram.data,
                        EMS_TxTelegram.length,
                        COLOR_CYAN);

    EMS_Sys_Status.emsTxStatus = EMS_TX_ACTIVE;
    emsuart_tx_buffer(EMS_TxTelegram.data, EMS_TxTelegram.length);
    EMS_Sys_Status.emsTxPkgs++;
    EMS_Sys_Status.emsLastTx = millis();

    // if it was a write command, check if we need to do a new read to validate the results
    if (EMS_TxTelegram.action == EMS_TX_WRITE) {
        // straight after the write check to see if we have to followup with a read to verify the write worked
        // we do this by re-submitting the same telegram but this time as a write command (type,offset,length are still valid)
        if (EMS_TxTelegram.type_validate != 0) {
            EMS_TxTelegram.dest = EMS_TxTelegram.dest | 0x80; // add the 7th bit make sure its a read
            _buildTxTelegram(1);                              // get a single value back, which is one byte
        }
        EMS_TxTelegram.action = EMS_TX_VALIDATE; // will start the validate next time a telegram is received
    } else {
        EMS_Sys_Status.emsTxStatus = EMS_TX_IDLE; // nothing to send
    }
}

/*
 * parse the telegram message
 * length is only data bytes, excluding the BRK
 * Read commands are asynchronous as they're handled by the interrupt
 * When we receive a Poll Request we need to send quickly
 */
void ems_parseTelegram(uint8_t * telegram, uint8_t length) {
    // if we're waiting on a reponse from a read and it hasn't come, try again
    if ((EMS_Sys_Status.emsTxStatus != EMS_TX_PENDING)
            && ((EMS_TxTelegram.action == EMS_TX_READ) || (EMS_TxTelegram.action == EMS_TX_VALIDATE))
            && ((millis() - EMS_Sys_Status.emsLastTx) > RX_READ_TIMEOUT)) {
        if (emsLastRxCount++ >= RX_READ_TIMEOUT_COUNT) {
            // give up
            myDebug("Error! no send acknowledgement. Giving up.\n");
            _initTxBuffer();
        } else {
            myDebug("Didn't receive acknowledgement so resending (attempt #%d/%d)...\n",
                    emsLastRxCount,
                    RX_READ_TIMEOUT_COUNT);
            EMS_Sys_Status.emsTxStatus = EMS_TX_PENDING; // set to pending will trigger sending the same package again
        }
    }

    // check if we just received one byte
    // it could be a Poll request from the boiler which is 0x8B (0x0B | 0x80 to set 7th bit)
    // or a return code like 0x01 or 0x04 from the last Write command
    if (length == 1) {
        uint8_t value = telegram[0]; // 1st byte

        // check first for Poll
        if (value == (EMS_ID_ME | 0x80)) {
            // set the timestamp of the last poll, we use this to see if we have a connection to the boiler
            EMS_Sys_Status.emsLastPoll = millis();

            // do we have something to send? if so send it
            if (EMS_Sys_Status.emsTxStatus == EMS_TX_PENDING) {
                _ems_sendTelegram();
            } else {
                // nothing to send so just send a poll acknowledgement back
                if (EMS_Sys_Status.emsPollEnabled) {
                    emsaurt_tx_poll();
                }
            }
            // check if we're waiting on a response after a recent Write, which is a return code (0x01 for success or 0x04 for error)
        } else if (EMS_TxTelegram.action == EMS_TX_WRITE) {
            // TODO: need to tidy this piece up!
            // a response from UBA after a write should be within a specific time period <100ms
            /*
            if (value == 0x01) {
                emsaurt_tx_poll(); // send a poll acknowledgement
                myDebug("Receiver of last write acknowledged OK, ready to validate...\n");
                EMS_TxTelegram.action = EMS_TX_VALIDATE;
            } else if (value == 0x04) {
                EMS_TxTelegram.action = EMS_TX_NONE;
                myDebug("Return value from write FAILED.\n");
                emsaurt_tx_poll(); // send a poll acknowledgement
            }
            */
        }

        return; // all done here, quit. if we haven't processes anything its a poll but not for us
    }

    // ignore anything that doesn't resemble a proper telegram package
    // minimal is 5 bytes, excluding CRC at the end
    if ((length < 5)) {
        _debugPrintTelegram("Noisy data:", telegram, length, COLOR_MAGENTA);
        return;
    }

    // Assume at this point we have something that vaguely resembles a telegram
    // see if we got a telegram as [src] [dest] [type] [offset] [data] [crc]
    // so is at least 6 bytes long and the CRC checks out (which is last byte)
    uint8_t crc = _crcCalculator(telegram, length);
    if (telegram[length - 1] != crc) {
        EMS_Sys_Status.emxCrcErr++;
        _debugPrintTelegram("Corrupt telegram:", telegram, length, COLOR_RED);
    } else {
        // go and do the magic
        _processType(telegram, length);
    }
}

/*
 * decipher the telegram packet
 * length is only data bytes, excluding the BRK
 */
void _processType(uint8_t * telegram, uint8_t length) {
    // extract the 4-byte header information
    uint8_t src = telegram[0] & 0x7F; // remove 8th bit as we deal with both reads and writes

    // if its an echo of ourselves from the master, ignore
    if (src == EMS_ID_ME) {
        _debugPrintTelegram("Telegram echo:", telegram, length, COLOR_BLUE);
    }

    // header
    uint8_t   dest = telegram[1];
    uint8_t   type = telegram[2];
    uint8_t * data = telegram + 4; // data block starts at position 5

    // for building a debug message
    char s[100];
    char color_s[20];
    char src_s[20];
    char dest_s[20];

    // scan through known types
    int  i         = 0;
    bool typeFound = false;
    while (i < MAX_TYPECALLBACK) {
        if ((EMS_Types[i].src == src) && (EMS_Types[i].type == type)) {
            // we have a match
            typeFound = true;
            // call callback to fetch the values from the telegram
            // ignoring the return value for now
            if ((EMS_Types[i].processType_cb) != (void *)NULL) {
                (void)EMS_Types[i].processType_cb(data, length);
            }
            break;
        }
        i++;
    }

    if (src == EMS_ID_BOILER) {
        strcpy(src_s, "Boiler");
    } else if (src == EMS_ID_THERMOSTAT) {
        strcpy(src_s, "Thermostat");
    }

    // was it sent specifically to us?
    if (dest == EMS_ID_ME) {
        strcpy(dest_s, "telegram for us");
        strcpy(color_s, COLOR_YELLOW);

        // did we actually ask for it from an earlier read/write request?
        // note when we issue a read command the responder (dest) has to return a telegram back immediately
        if ((EMS_TxTelegram.action == EMS_TX_READ) && (EMS_TxTelegram.type == type) && typeFound) {
            // yes we were expecting this one one
            EMS_Sys_Status.emsRxPgks++; // increment rx counter
            EMS_Sys_Status.emsLastRx = millis();
            EMS_TxTelegram.action    = EMS_TX_NONE;
            emsLastRxCount           = 0; // reset retry count
        }
        // send Acknowledgement back to free the bus
        emsaurt_tx_poll();
    } else if (dest == EMS_ID_NONE) {
        // it's probably just a broadcast
        strcpy(dest_s, "broadcast");
        strcpy(color_s, COLOR_GREEN);
    } else {
        // for someone else
        strcpy(dest_s, "(not for us)");
        strcpy(color_s, COLOR_MAGENTA);
    }

    // debug print
    sprintf(s, "%s %s, type 0x%02x", src_s, dest_s, type);
    _debugPrintTelegram(s, telegram, length, color_s);
    if (typeFound) {
        myDebug("--> %s(0x%02x) received.\n", EMS_Types[i].typeString, type);
    }

    // check to see if we're waiting on a specific value, either from a recent read or by chance a broadcast
    // and then do the comparison
    if ((EMS_TxTelegram.action == EMS_TX_VALIDATE) && (EMS_TxTelegram.type == type)) {
        uint8_t offset;

        // we're waiting for a read on our last write to compare the values
        // if we have a special telegram package we sent to validate use that for comparison, otherwise assume its a simple 1-byte read
        if (EMS_TxTelegram.type_validate == EMS_ID_NONE) {
            offset = EMS_TxTelegram.offset; // special, use the offset from the last send
        } else {
            offset = 0;
        }

        // get the data at the position we wrote too
        // do compare, when validating we always return a single value
        if (EMS_TxTelegram.checkValue == data[offset]) {
            myDebug("Last write operation successful (value=%d, offset=%d)\n", EMS_TxTelegram.checkValue, offset);
            EMS_Sys_Status.emsRefreshed = true;        // flag this so values are sent back to HA via MQTT
            EMS_TxTelegram.action       = EMS_TX_NONE; // no more sends
        } else {
            myDebug("Last write operation failed. (value=%d, got=%d, offset=%d)\n",
                    EMS_TxTelegram.checkValue,
                    data[offset],
                    offset);
        }
    }
}

/*
 * UBAParameterWW - type 0x33 - warm water parameters
 */
bool _process_UBAParameterWW(uint8_t * data, uint8_t length) {
    EMS_Boiler.wWSelTemp     = data[2];
    EMS_Boiler.wWActivated   = (data[1] == 0xFF);
    EMS_Boiler.wWCircPump    = (data[6] == 0xFF);
    EMS_Boiler.wWDesiredTemp = data[8];

    return true;
}

/*
 * UBAMonitorWWMessage - type 0x34 - warm water monitor. 19 bytes long
 */
bool _process_UBAMonitorWWMessage(uint8_t * data, uint8_t length) {
    EMS_Boiler.wWCurTmp  = _toFloat(1, data);
    EMS_Boiler.wWStarts  = _toLong(13, data);
    EMS_Boiler.wWWorkM   = _toLong(10, data);
    EMS_Boiler.wWOneTime = bitRead(data[5], 1);

    return true;
}

/*
 * UBAMonitorFast - type 0x18 - central heating monitor part 1 (25 bytes long)
 */
bool _process_UBAMonitorFast(uint8_t * data, uint8_t length) {
    EMS_Boiler.selFlowTemp = data[0];
    EMS_Boiler.curFlowTemp = _toFloat(1, data);
    EMS_Boiler.retTemp     = _toFloat(13, data);

    uint8_t v          = data[7];
    EMS_Boiler.burnGas = bitRead(v, 0);
    EMS_Boiler.fanWork = bitRead(v, 2);
    EMS_Boiler.ignWork = bitRead(v, 3);
    EMS_Boiler.heatPmp = bitRead(v, 5);
    EMS_Boiler.wWHeat  = bitRead(v, 6);
    EMS_Boiler.wWCirc  = bitRead(v, 7);

    EMS_Boiler.selBurnPow = data[3];
    EMS_Boiler.curBurnPow = data[4];

    EMS_Boiler.flameCurr = _toFloat(15, data);

    if (data[17] == 0xFF) { // missing value for system pressure
        EMS_Boiler.sysPress = 0;
    } else {
        EMS_Boiler.sysPress = (((float)data[17]) / (float)10);
    }

    return true;
}

/*
 * UBAMonitorSlow - type 0x19 - central heating monitor part 2 (27 bytes long)
 */
bool _process_UBAMonitorSlow(uint8_t * data, uint8_t length) {
    EMS_Boiler.extTemp     = _toFloat(0, data); // 0x8000 if not available
    EMS_Boiler.boilTemp    = _toFloat(2, data); // 0x8000 if not available
    EMS_Boiler.pumpMod     = data[13];
    EMS_Boiler.burnStarts  = _toLong(10, data);
    EMS_Boiler.burnWorkMin = _toLong(13, data);
    EMS_Boiler.heatWorkMin = _toLong(19, data);

    return true;
}

/*
 * RC20StatusMessage - type 0x91 - data from the RC20 thermostat (0x17) - 15 bytes long
 */
bool _process_RC20StatusMessage(uint8_t * data, uint8_t length) {
    EMS_Thermostat.setpoint_roomTemp = ((float)data[1]) / (float)2;
    EMS_Thermostat.curr_roomTemp     = _toFloat(2, data);

    // set the updated flag to trigger a send back to HA
    EMS_Sys_Status.emsRefreshed = true;

    return true;
}

/*
 * RC20Temperature - type 0xa8 - set temp value from the RC20 thermostat (0x17)
 * Special case as we only want to store the value after a write command
 */
bool _process_RC20Temperature(uint8_t * data, uint8_t length) {
    // only interested in the single byte response we send to validate a temp change
    if (length == 6) {
        EMS_Thermostat.setpoint_roomTemp = ((float)data[0]) / (float)2;

        EMS_Sys_Status.emsRefreshed = true; // set the updated flag to trigger a send back to HA
    }

    return true;
}

/*
 * process_RC20Time - type 0x06 - date and time from the RC20 thermostat (0x17) - 14 bytes long
 */
bool _process_RC20Time(uint8_t * data, uint8_t length) {
    EMS_Thermostat.hour   = data[2];
    EMS_Thermostat.minute = data[4];
    EMS_Thermostat.second = data[5];
    EMS_Thermostat.day    = data[3];
    EMS_Thermostat.month  = data[1];
    EMS_Thermostat.year   = data[0];

    // now we have the time, set our ESP code to it - wil be replaced with NTP
    setTime(EMS_Thermostat.hour,
            EMS_Thermostat.minute,
            EMS_Thermostat.second,
            EMS_Thermostat.day,
            EMS_Thermostat.month,
            EMS_Thermostat.year + 2000);

    return true;
}

/*
 *  Build the telegram, which includes a single byte followed by the CRC at the end
 */
void _buildTxTelegram(uint8_t data_value) {
    // header
    EMS_TxTelegram.data[0] = EMS_ID_ME;             // src
    EMS_TxTelegram.data[1] = EMS_TxTelegram.dest;   // dest
    EMS_TxTelegram.data[2] = EMS_TxTelegram.type;   // type
    EMS_TxTelegram.data[3] = EMS_TxTelegram.offset; //offset

    // data
    EMS_TxTelegram.data[4] = data_value; // value, can be size

    // crc
    EMS_TxTelegram.data[5] = _crcCalculator(EMS_TxTelegram.data, EMS_TxTelegram.length);

    EMS_Sys_Status.emsTxStatus = EMS_TX_PENDING; // armed and ready to send
}


/*
 * Send a command to Tx to Read from another device
 * Read commands when sent must to responded too by the destination (target) immediately
 * usually within a 10ms window
 */
void ems_doReadCommand(uint8_t type) {
    if (type == EMS_TYPE_NONE)
        return; // not a valid type, quit

    // check if there is already something in the queue
    if (EMS_Sys_Status.emsTxStatus == EMS_TX_PENDING) { // send is already pending
        myDebug("Cannot write - already a telegram pending send.\n");
        return;
    }

    uint8_t dest, size;

    // scan through known types
    bool typeFound = false;
    int  i         = 0;
    while (i < MAX_TYPECALLBACK) {
        if (EMS_Types[i].type == type) {
            typeFound = true; // we have a match
            // call callback to fetch the values from the telegram
            dest = EMS_Types[i].src;
            size = EMS_Types[i].size;
            break;
        }
        i++;
    }

    // for adhoc calls use default values
    if (!typeFound) {
        dest = EMS_ID_BOILER; // default is boiler
        size = 1;
        myDebug("Requesting type (0x%02x) from dest 0x%02x for %d bytes\n", type, dest, size);
    } else {
        myDebug("Requesting type %s(0x%02x) from dest 0x%02x for %d bytes\n", EMS_Types[i].typeString, type, dest, size);
    }

    EMS_TxTelegram.action = EMS_TX_READ; // read command
    EMS_TxTelegram.dest   = dest | 0x80; // set 7th bit to indicate a read
    EMS_TxTelegram.offset = 0;           // 0 for all data
    EMS_TxTelegram.length = 6;           // is always 6 bytes long (including CRC at end)
    EMS_TxTelegram.type   = type;

    _buildTxTelegram(size); // we send the # bytes we want back
}

/*
 * Set the temperature of the thermostat
 */
void ems_setThermostatTemp(float temperature) {
    // check if there is already something in the queue
    if (EMS_Sys_Status.emsTxStatus == EMS_TX_PENDING) { // send is already pending
        myDebug("Cannot write - already a telegram pending send.\n");
        return;
    }
    char s[10];
    myDebug("Setting thermostat temperature to %s C\n", _float_to_char(s, temperature));

    EMS_TxTelegram.action     = EMS_TX_WRITE; // write command
    EMS_TxTelegram.dest       = EMS_ID_THERMOSTAT;
    EMS_TxTelegram.type       = EMS_TYPE_RC20Temperature;
    EMS_TxTelegram.offset     = 0x1C;                                     // manual setpoint temperature
    EMS_TxTelegram.length     = 6;                                        // includes CRC
    EMS_TxTelegram.checkValue = (uint8_t)((float)temperature * (float)2); // value to compare against. must be a single int

    // post call is RC20StatusMessage to fetch temps and send to HA
    EMS_TxTelegram.type_validate = EMS_TYPE_RC20Temperature;

    _buildTxTelegram(EMS_TxTelegram.checkValue);
}

/*
 * Set the warm water temperature
 */
void ems_setWarmWaterTemp(uint8_t temperature) {
    // check if there is already something in the queue
    if (EMS_Sys_Status.emsTxStatus == EMS_TX_PENDING) { // send is already pending
        myDebug("Cannot write - already a telegram pending send.\n");
        return;
    }

    myDebug("Setting boiler warm water temperature to %d C\n", temperature);

    EMS_TxTelegram.action     = EMS_TX_WRITE; // write command
    EMS_TxTelegram.dest       = EMS_ID_BOILER;
    EMS_TxTelegram.type       = EMS_TYPE_UBAParameterWW;
    EMS_TxTelegram.offset     = 0x02;        // Temperature
    EMS_TxTelegram.length     = 6;           // includes CRC
    EMS_TxTelegram.checkValue = temperature; // value to compare against. must be a single int

    EMS_TxTelegram.type_validate = EMS_ID_NONE; // this means don't send and we'll pick up the data from the next broadcast

    _buildTxTelegram(temperature);
}

/*
 * Activate / De-activate the Warm Water
 * true = on, false = off
 */
void ems_setWarmWaterActivated(bool activated) {
    // check if there is already something in the queue
    if (EMS_Sys_Status.emsTxStatus == EMS_TX_PENDING) { // send is already pending
        myDebug("Cannot write - already a telegram pending send.\n");
        return;
    }

    myDebug("Setting boiler warm water to %s\n", activated ? "on" : "off");

    EMS_TxTelegram.action        = EMS_TX_WRITE; // write command
    EMS_TxTelegram.dest          = EMS_ID_BOILER;
    EMS_TxTelegram.type          = EMS_TYPE_UBAParameterWW;
    EMS_TxTelegram.offset        = 0x01;        // WW activation
    EMS_TxTelegram.length        = 6;           // includes CRC
    EMS_TxTelegram.type_validate = EMS_ID_NONE; // this means don't send and we'll pick up the data from the next broadcast

    if (activated) {
        EMS_TxTelegram.checkValue = 0xFF; // the EMS value for on
        _buildTxTelegram(0xFF);
    } else {
        EMS_TxTelegram.checkValue = 0x00; // the EMS value for off
        _buildTxTelegram(0x00);
    }
}


/*
 * Helper functions for formatting and converting floats
 */

// function to turn a telegram int (2 bytes) to a float
float _toFloat(uint8_t i, uint8_t * data) {
    if ((data[i] == 0x80) && (data[i + 1] == 0)) // 0x8000 is used when sensor is missing
        return (float)-1;

    return ((float)(((data[i] << 8) + data[i + 1]))) / 10;
}

// function to turn a telegram long (3 bytes) to a long int
uint16_t _toLong(uint8_t i, uint8_t * data) {
    return (((data[i]) << 16) + ((data[i + 1]) << 8) + (data[i + 2]));
}

// convert float to char
char * _float_to_char(char * a, float f, uint8_t precision) {
    long p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    char * ret = a;
    // check for 0x8000 (sensor missing)
    if (f == -1) {
        strcpy(ret, "<n/a>");
    } else {
        long whole = (long)f;
        itoa(whole, a, 10);
        while (*a != '\0')
            a++;
        *a++         = '.';
        long decimal = abs((long)((f - whole) * p[precision]));
        itoa(decimal, a, 10);
    }
    return ret;
}
