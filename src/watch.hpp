#include "driver/gpio.h"
#define ENABLE_POWER
#define POWER_MONITOR_CHARGE_STATE GPIO_NUM_32
#define POWER_MONITOR_VBATT GPIO_NUM_35
#define POWER_MONITOR_VBUS GPIO_NUM_36
#define POWER_MONITOR_DIVISOR 2

#define ENABLE_I2C
#define I2C_SDA GPIO_NUM_21
#define I2C_SCL GPIO_NUM_22
#define I2C_CLOCK I2C_FREQ_DEFAULT

#define ENABLE_SPI
#define SPI_SCLK GPIO_NUM_18
#define SPI_MOSI GPIO_NUM_19
#define SPI_SS GPIO_NUM_5

#define ENABLE_TTP223
#define PIN_INTERRUPT_TTP223 GPIO_NUM_33
#define PIN_POWER_TTP223 GPIO_NUM_25

#define ENABLE_PCF8563
#define PIN_INTERRUPT_PCF8563 GPIO_NUM_34

#define ENABLE_MPU9250
#define PIN_INTERRUPT_MPU9250 GPIO_NUM_39

#define ENABLE_RTC

#define ENABLE_BLUETOOTH

#define ENABLE_WIFI
#define WIFI_POWER_SAVING WIFI_PS_MAX_MODEM

#define ENABLE_ST7735
#define DISPLAY_WIDTH 160
#define DISPLAY_HEIGHT 80
#define DISPLAY_OFFSET_COLS 40
#define DISPLAY_OFFSET_ROWS 0
#define ST77XX_CS GPIO_NUM_5
#define ST77XX_DC GPIO_NUM_23
#define ST77XX_RST GPIO_NUM_26
#define ST77XX_BL GPIO_NUM_27

#define ENABLE_CPU_SCALER
#define CPU_FREQ_ONDEMAND 240

#define ENABLE_SERVICE_NTP

#define ENABLE_SERVICE_SLEEPTUNE
#define SLEEPTUNE_LOOPS_PER_SECOND 10

#define ENABLE_MONOCOLOUR_LED GPIO_NUM_4

//#define DEBUG_LOOP_RUNNING

#include <kern.hpp>
#include "watchService.hpp"

class Watch : public sDOS
{
    public:
        void Setup(){
            sDOS::Setup();
            
            _watchService = new WatchService(
                _debugger, _events, _driver_RTC, 
                _driver_WiFi, _driver_BT , _cpuScaler, 
                _driver_FrameBuffer
            );
            _debugger.Debug(_component, "just before watch service setup");
            _watchService->setup();
            _debugger.Debug(_component, "just before services.push_back");
            _services.push_back(_watchService);
        };
    protected:
        WatchService * _watchService;
};