#include "hardwares.h"
#include <kern.hpp>
#include "t-watch.watchService.hpp"

class Watch : public sDOS {
public:
    void Setup() {
        // Instantiate sDOS core.
        sDOS::Setup();

        // Create OTA service instance
        _ota = new sDOS_OTA_Service(_debugger, _events, _driver_WiFi, _cpuScaler);

        // Create our watch service
#ifdef HARDWARE_M5STACK_C
#endif
#ifdef HARDWARE_TWATCH
        _watchService = new TWatch_WatchService(
            _debugger,
            _events,
            _driver_RTC,
            _driver_WiFi,
            _driver_BT,
            _ota,
            _cpuScaler,
            _driver_FrameBuffer,
            _button_ttp223,
            _mono_led
        );
#endif

        // Since we're too late for the setup now, we gotta run it ourselves
        //_watchService->setup();

        // Add our watchService et al to sDOS core. We can add drivers like this too.
        sDOS::add(_ota);
        sDOS::add(_watchService);
    };

protected:
#ifdef HARDWARE_M5STACK_C
#endif
#ifdef HARDWARE_TWATCH
    TWatch_WatchService *_watchService;
#endif
    sDOS_OTA_Service * _ota;
};