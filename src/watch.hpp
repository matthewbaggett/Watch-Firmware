#include "hardwares.h"
#include <kern.hpp>
#ifdef HARDWARE_M5STACK_C
#include "watchService/m5stackc.hpp"
#endif
#ifdef HARDWARE_TWATCH
#include "watchService/t-watch.hpp"
#endif

class Watch : public sDOS {
public:
    Watch() : sDOS() {
    }
    void setup() {
        sDOS::setup();
        // Create OTA service instance
        //_ota = new sDOS_OTA_Service(_debugger, _eventsManager, _driver_WiFi, _cpuScaler);

        // Create our watch service
#ifdef HARDWARE_M5STACK_C
        _watchService = new M5StackC_WatchService(
            _debugger,
            _eventsManager,
            _driver_FrameBuffer,
            _driver_RTC,
            _driver_WiFi,
            _driver_BT,
            _cpuScaler,
            _ota
        );
#endif
#ifdef HARDWARE_TWATCH
        _watchService = new TWatch_WatchService(
            _debugger,
            _eventsManager,
            _driver_FrameBuffer,
            _driver_RTC,
            _driver_WiFi,
            _driver_BT,
            _cpuScaler,
            _ota,
            _button_ttp223,
            _mono_led
        );
#endif

        // Since we're too late for the setup now, we gotta run it ourselves
        Serial.println("Watch::setup() A");
        _watchService->setup();

        // Add our watchService et al to sDOS core. We can add drivers like this too.
        Serial.println("Watch::setup() B");

        //sDOS::add(_ota);
        Serial.println("Watch::setup() C");
        //sDOS::add(_watchService);
        Serial.println("Watch::setup() D");
    };

protected:
#ifdef HARDWARE_M5STACK_C
    M5StackC_WatchService *_watchService;
#endif
#ifdef HARDWARE_TWATCH
    TWatch_WatchService *_watchService;
#endif
    sDOS_OTA_Service * _ota;
};