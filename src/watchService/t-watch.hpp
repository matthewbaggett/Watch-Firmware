#include "kern_inc.h"
#include "watchService.hpp"

#include "abstracts/service.hpp"
#include <NTPClient.h>
#include <debugger.hpp>
#include <events.hpp>
#include <abstracts/rtc.hpp>
#include <drivers/WiFiManager.hpp>
#include <drivers/BluetoothManager.hpp>
#include <services/cpu_scaler.hpp>
#include <drivers/display/frame_buffer.hpp>
#include <cstdlib>
#include <string>

class TWatch_WatchService : public WatchService {
public:
    TWatch_WatchService(
        Debugger * debugger,
        EventsManager * eventsManager,
        sDOS_FrameBuffer * frameBuffer,
        sDOS_PCF8563 *rtc,
        WiFiManager *wifi,
        BluetoothManager *bt,
        sDOS_OTA_Service *ota,
        sDOS_CPU_SCALER *cpuScaler,
        sDOS_TTP223 *button,
        sDOS_LED_MONO *led
    ) : WatchService(debugger, eventsManager, frameBuffer) {

        debugger->Debug("test", "arse");
        _rtc = rtc;
        _wifi = wifi;
        _bt = bt;
        _ota = ota;
        _cpuScaler = cpuScaler;
        _button = button;
        _led = led;

    };


};
