#include "hardwares.h"
#include <kern.hpp>
#include "watchService.hpp"

class Watch : public sDOS {
public:
    void Setup() {
        // Instantiate sDOS core.
        sDOS::Setup();

        // Create OTA service instance
        _ota = new sDOS_OTA_Service(_debugger, _events, _driver_WiFi, _cpuScaler);

        // Create our watch service
        _watchService = new WatchService(
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

        // Since we're too late for the setup now, we gotta run it ourselves
        _watchService->setup();

        // Add our watchService et al to sDOS core. We can add drivers like this too.
        sDOS::add(_ota);
        sDOS::add(_watchService);
    };

protected:
    WatchService *_watchService;
    sDOS_OTA_Service * _ota;
};