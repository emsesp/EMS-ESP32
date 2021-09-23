extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include "console.h"
#include "uuid/log.h"

namespace emsesp {

class PoolPump {
  public:
    PoolPump(uint8_t gpio, uint16_t delay);
    virtual ~PoolPump();
    static void StartPump();
    static void StopPump();
    void SetGPIO(uint8_t gpio);
    void SetDelay(uint16_t delay);
    void setPump(bool state,bool force);

    static TimerHandle_t        m_hPumpStartTimer;
    static TimerHandle_t        m_hPumpStopTimer;
    static TimerHandle_t        m_hPumpForceTimer;
	static uint8_t			    m_bPumpOn;
  
  private:
    static uuid::log::Logger    logger_;
    static bool                 m_bInitialized;
    static uint8_t              m_nGPIO;
    static uint16_t             m_nDelay;
};

}