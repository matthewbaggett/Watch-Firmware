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
#include <drivers/rtc/fake.hpp>
#include <drivers/display/frame_buffer.hpp>
#include <cstdlib>
#include <string>

class M5StackC_WatchService : public WatchService {
public:
    M5StackC_WatchService(
        Debugger * debugger,
        EventsManager * eventsManager,
        sDOS_FrameBuffer *frameBuffer,
        sDOS_FAKE_RTC *rtc,
        WiFiManager *wifi,
        BluetoothManager *bt,
        sDOS_OTA_Service *ota,
        sDOS_CPU_SCALER *cpuScaler,
    ) : WatchService(debugger, eventsManager, frameBuffer) {

        _rtc = rtc;
        _wifi = wifi;
        _bt = bt;
        _ota = ota;
        _cpuScaler = cpuScaler;

    };


};
