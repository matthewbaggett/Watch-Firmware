#include "kern_inc.h"
#include "abstracts/service.hpp"
#include <NTPClient.h>
#include <debugger.hpp>
#include <events.hpp>
#include <abstracts/rtc.hpp>
#include <wifi.hpp>
#include <bluetooth/bluetooth.hpp>
#include <services/cpu_scaler.hpp>
#include <drivers/display/frame_buffer.hpp>

#ifndef NTP_UPDATE_INTERVAL_SECONDS
#define NTP_UPDATE_INTERVAL_SECONDS 60 * 60 * 12
#endif

class WatchService : public sDOS_Abstract_Service {
public:
    WatchService(Debugger &debugger, EventsManager &eventsManager, AbstractRTC *rtc, WiFiManager *wifi,
                 BluetoothManager *bt, sDOS_CPU_SCALER *cpuScaler, sDOS_FrameBuffer *frameBuffer, sDOS_TTP223 *button,
                 sDOS_LED_MONO *led)
            : _debugger(debugger), _eventsManager(eventsManager), _rtc(rtc), _wifi(wifi), _bt(bt),
              _cpuScaler(cpuScaler), _frameBuffer(frameBuffer), _button(button), _led(led) {};

    void setup() {
        _debugger.Debug(_component, "setup begin");
        _frameBuffer->setAll(0, 0, 255);
        _eventsManager.on(F("TTP223_down"), &WatchService::faciaButtonPressCallback);
        _debugger.Debug(_component, "setup over");
    };

    void loop() {
        if (WatchService::faciaButtonPressed) {
            WatchService::faciaButtonPressed = false;
            touch();
        }
    };

    void touch() {
        if (_bleh) {
            _bleh = false;
            _frameBuffer->setAll(255, 0, 0);
            _led->setBrightness(0);
        } else {
            _bleh = true;
            _frameBuffer->setAll(0, 255, 0);
            _led->setBrightness(255);
        }
    };

    String getName() { return _component; };

    bool isActive() { return true; };

    static void faciaButtonPressCallback(String payload) {
        WatchService::faciaButtonPressed = true;
    };

    static bool faciaButtonPressed;

private:
    String _component = "timepiece";
    Debugger _debugger;
    EventsManager _eventsManager;
    AbstractRTC *_rtc;
    WiFiManager *_wifi;
    BluetoothManager *_bt;
    sDOS_CPU_SCALER *_cpuScaler;
    sDOS_FrameBuffer *_frameBuffer;
    sDOS_TTP223 *_button;
    sDOS_LED_MONO *_led;
    bool _bleh = false;
};

bool WatchService::faciaButtonPressed = false;
