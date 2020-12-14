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
        // Create OTA service instance
        //_ota = new sDOS_OTA_Service(_debugger, _eventsManager, _driver_WiFi, _cpuScaler);

        // Create our watch service
#ifdef HARDWARE_M5STACK_C__
        _watchService = new M5StackC_WatchService(
            _debugger,
            _eventsManager,
            _driver_FrameBuffer
            _driver_RTC,
            _driver_WiFi,
            _driver_BT,
            _ota,
            _cpuScaler,
        );
#endif
#ifdef HARDWARE_TWATCH__
        _watchService = new TWatch_WatchService(
            _debugger,
            _eventsManager,
            _driver_FrameBuffer
            _driver_RTC,
            _driver_WiFi,
            _driver_BT,
            _ota,
            _cpuScaler,
            _button_ttp223,
            _mono_led
        );
#endif

        // Since we're too late for the setup now, we gotta run it ourselves
        //_watchService->setup();

        // Add our watchService et al to sDOS core. We can add drivers like this too.
        //sDOS::add(_ota);
        //sDOS::add(_watchService);
        Serial.println("End of Watch::Watch()");
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