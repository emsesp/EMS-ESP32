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

PoolPump::PoolPump(uint8_t gpio, uint16_t delay) {
    if (m_bInitialized) {
        if (gpio != m_nGPIO) SetGPIO(gpio);
        xTimerChangePeriod(m_hPumpStopTimer, pdMS_TO_TICKS(1000*delay),0);
    }
    else {
        m_bInitialized = true;
        LOG_INFO("Initializing pump timer and gpio");
        m_hPumpStartTimer = xTimerCreate("pumpStartTimer", pdMS_TO_TICKS(1000*6), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(StartPump));
        m_hPumpStopTimer = xTimerCreate("pumpStopTimer", pdMS_TO_TICKS(1000*delay), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(StopPump));
        m_hPumpForceTimer = xTimerCreate("pumpForceTimer", pdMS_TO_TICKS(1000*60*30), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(StopPump));
        m_nDelay = delay;
        SetGPIO(gpio);
    }
    m_nDelay = delay;

}

PoolPump::~PoolPump() {
    if (m_bInitialized) {
        StopPump();
        xTimerStop(m_hPumpStartTimer, 0);
        xTimerStop(m_hPumpStopTimer, 0);
        xTimerStop(m_hPumpForceTimer, 0);
    }  
} 

void PoolPump::SetGPIO(uint8_t gpio) {
    LOG_INFO("Setting pump gpio to: %d",gpio);
    if (m_nGPIO == gpio) return;
    pinMode(gpio,OUTPUT);
    m_nGPIO = gpio;
}

void PoolPump::SetDelay(uint16_t delay) {
    if (!m_bInitialized) return;
    if (m_nDelay == delay) return;
    LOG_INFO("Setting pump stop delay to: %d",delay);
    xTimerChangePeriod(m_hPumpStopTimer, pdMS_TO_TICKS(1000*delay),0);
}

void PoolPump::setPump(bool state,bool force) {
    if (!m_bInitialized) return;
    LOG_INFO("Set Pump %s with force: %s",state ? "on" : "off",force ? "true" : "false");
    if (state) {
        if (force) {
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
    if (!m_bInitialized) return;
    if (m_bPumpOn == 0x00) LOG_INFO("Starting pump");
    m_bPumpOn = 0xff;
    digitalWrite(m_nGPIO,HIGH);
}
void PoolPump::StopPump() {
    if (!m_bInitialized) return;
    if (m_bPumpOn == 0xff) LOG_INFO("Stopping pump");
    m_bPumpOn = 0x00;
    digitalWrite(m_nGPIO,LOW);
}

}
