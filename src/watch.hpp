#include "hardware.h"
#include <kern.hpp>
#include "watchService.hpp"

class Watch : public sDOS
{
    public:
        void Setup(){
            sDOS::Setup();
            
            _watchService = new WatchService(
                _debugger,
                _events,
                _driver_RTC,
                _driver_WiFi,
                _driver_BT ,
                _cpuScaler,
                _driver_FrameBuffer,
                _button_ttp223,
                _mono_led
            );
            _debugger.Debug(_component, "just before watch service setup");
            _watchService->setup();
            _debugger.Debug(_component, "just before services.push_back");
            _services.push_back(_watchService);
        };
    protected:
        WatchService * _watchService;
};