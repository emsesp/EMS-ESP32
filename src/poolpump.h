#ifndef EMSESP_POOLPUMP_H_
#define EMSESP_POOLPUMP_H_

extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include "console.h"
#include "uuid/log.h"

namespace emsesp {

class PoolPump {
  public:
    static void Initialize();
    static void Delete();
    static void StartPump();
    static void StopPump();
    static void SetGPIO(uint8_t gpio);
    static void SetDelay(uint16_t delay);
    static void SetValues(uint8_t gpio,uint16_t delay);
    static void setPump(bool state,bool force = false);

    static TimerHandle_t        m_hPumpStartTimer;
    static TimerHandle_t        m_hPumpStopTimer;
    static TimerHandle_t        m_hPumpForceTimer;
	  static uint8_t			        m_bPumpOn;
  
  private:
    static uuid::log::Logger    logger_;
    static bool                 m_bInitialized;
    static uint8_t              m_nGPIO;
    static uint16_t             m_nDelay;
};

}

#endif