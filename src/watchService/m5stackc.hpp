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
        sDOS_FrameBuffer * frameBuffer,
        sDOS_RTC * rtc,
        WiFiManager * wifi,
        BluetoothManager * bt,
        sDOS_CPU_SCALER * cpuScaler,
        sDOS_OTA_Service * ota,
        sDOS_AXP192 * axp192
    ) :
        WatchService(debugger, eventsManager, frameBuffer, rtc, wifi, bt, cpuScaler, ota), _axp192(axp192)
    {
        debugger->Debug(this->_component, "M5StackC_WatchService entered");
    };

    void setup() {

        this->_debugger->Debug(this->_component, "Defining AXP192 InitDef");

        sDOS_AXP192_InitDef initDef = {
                .EXTEN  = true,
                .BACKUP = true,
                .DCDC1  = 3300,
                .DCDC2  = 0,
                .DCDC3  = 0,
                .LDO2   = 3000,
                .LDO3   = 3000,
                .GPIO0  = 2800,
                .GPIO1  = -1,
                .GPIO2  = -1,
                .GPIO3  = -1,
                .GPIO4  = -1,
        };

        this->_debugger->Debug(this->_component, "AXP192 InitDef defined");

        this->_axp192->begin(initDef);

        this->_debugger->Debug(this->_component, "AXP192 Initialised");
    }
protected:
    sDOS_AXP192 * _axp192;

};
