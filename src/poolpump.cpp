#include <Arduino.h>
#include "emsesp.h"

namespace emsesp { 

uuid::log::Logger PoolPump::logger_{F_(pump), uuid::log::Facility::CONSOLE};
bool PoolPump::m_bInitialized = false;
uint8_t PoolPump::m_bPumpOn = 0x00;
uint8_t PoolPump::m_nGPIO = 0xff;
uint16_t PoolPump::m_nDelay = 300;
xTimerHandle PoolPump::m_hPumpStartTimer = NULL;
xTimerHandle PoolPump::m_hPumpStopTimer = NULL;
xTimerHandle PoolPump::m_hPumpForceTimer = NULL;

void PoolPump::Initialize() {
    if (m_bInitialized) return;
    LOG_INFO("Initializing pump timers");
    m_hPumpStartTimer = xTimerCreate("pumpStartTimer", pdMS_TO_TICKS(1000*6), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(StartPump));
    m_hPumpStopTimer = xTimerCreate("pumpStopTimer", pdMS_TO_TICKS(1000*m_nDelay), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(StopPump));
    m_hPumpForceTimer = xTimerCreate("pumpForceTimer", pdMS_TO_TICKS(1000*60*30), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(StopPump));
    m_bInitialized = true;
}

void PoolPump::Delete() {
    if (m_bInitialized) {
        StopPump();
        LOG_INFO("Deleting pump and uninitialize timers");
        xTimerStop(m_hPumpStartTimer, 0);
        xTimerStop(m_hPumpStopTimer, 0);
        xTimerStop(m_hPumpForceTimer, 0);
        xTimerDelete(m_hPumpStartTimer, 0);
        xTimerDelete(m_hPumpStopTimer, 0);
        xTimerDelete(m_hPumpForceTimer, 0);
        m_bPumpOn = EMS_VALUE_BOOL_NOTSET;
        m_bInitialized = false;
    }
    
}

void PoolPump::SetValues(uint8_t gpio, uint16_t delay) {
    if (m_nGPIO != gpio) SetGPIO(gpio);   
    if (m_nDelay != delay) SetDelay(delay);
    if (!m_bInitialized) m_bPumpOn = 0x00;
}

void PoolPump::SetGPIO(uint8_t gpio) {
    LOG_INFO("Setting pump gpio to: %d",gpio);
    if (m_nGPIO == gpio) return;
    pinMode(gpio,OUTPUT);
    digitalWrite(gpio,LOW);
    m_nGPIO = gpio;
}

void PoolPump::SetDelay(uint16_t delay) {
    if (m_nDelay == delay) return;
    m_nDelay = delay;
    LOG_INFO("Setting pump stop delay to: %d",delay);
    if (!m_bInitialized) return;
    xTimerChangePeriod(m_hPumpStopTimer, pdMS_TO_TICKS(1000*delay),0);
}

void PoolPump::setPump(bool state,bool force) {
    if (!m_bInitialized) Initialize();
    LOG_INFO("Set Pump %s with force: %s",state ? "on" : "off",force ? "true" : "false");
    if (state) {
        if (force) {
            if (m_bPumpOn == 0xFF) return;
            xTimerStop(m_hPumpStopTimer, 0);
            xTimerStop(m_hPumpForceTimer, 0);
            xTimerStart(m_hPumpForceTimer, 0);
            StartPump();
        }
        else  {
            xTimerStop(m_hPumpStopTimer, 0);
            xTimerStart(m_hPumpStartTimer, 0);
            xTimerStop(m_hPumpForceTimer, 0);
        }
    }
    else {
        if (force) {
            if (!xTimerIsTimerActive(m_hPumpForceTimer)) return;
            xTimerStop(m_hPumpForceTimer, 0);
            StopPump();
        }
        else {
            if (xTimerIsTimerActive(m_hPumpForceTimer)) return;
            xTimerStop(m_hPumpStartTimer, 0);
            xTimerStart(m_hPumpStopTimer, 0);

        }

    }
}

void PoolPump::StartPump() {
    if (!m_bInitialized) Initialize();
    if (m_bPumpOn == 0x00) LOG_INFO("Starting pump (%d seconds)",m_nDelay);
    m_bPumpOn = 0xff;
    digitalWrite(m_nGPIO,HIGH);
}
void PoolPump::StopPump() {
    if (!m_bInitialized) Initialize();
    if (m_bPumpOn == 0xff) LOG_INFO("Stopping pump");
    m_bPumpOn = 0x00;
    digitalWrite(m_nGPIO,LOW);
}

}
