#include "kern_inc.h"
#include "abstracts/service.hpp"
#include <NTPClient.h>
#include <debugger.hpp>
#include <events.hpp>
#include <abstracts/rtc.hpp>
#include <drivers/WiFiManager.hpp>
#include <drivers/BluetoothManager.hpp>
#include <services/cpu_scaler.hpp>
#include <drivers/display/frame_buffer.hpp>
#include <stdlib.h>
#include <string>

#ifndef NTP_UPDATE_INTERVAL_SECONDS
#define NTP_UPDATE_INTERVAL_SECONDS 60 * 60 * 12
#endif

class WatchService : public sDOS_Abstract_Service {
public:
    WatchService(Debugger &debugger, EventsManager &eventsManager, sDOS_PCF8563 *rtc, WiFiManager *wifi,
                 BluetoothManager *bt, sDOS_OTA_Service *ota, sDOS_CPU_SCALER *cpuScaler, sDOS_FrameBuffer *frameBuffer, sDOS_TTP223 *button,
                 sDOS_LED_MONO *led)
            : _debugger(debugger), _eventsManager(eventsManager), _rtc(rtc), _wifi(wifi), _bt(bt), _ota(ota),
              _cpuScaler(cpuScaler), _frameBuffer(frameBuffer), _button(button), _led(led) {
        // Setup events listeners
        EventsManager::on(F("TTP223_down"), &WatchService::faciaButtonPressCallback);
        EventsManager::on(F("wifi_on"), &WatchService::wifiStateChangeOn);
        EventsManager::on(F("wifi_off"), &WatchService::wifiStateChangeOff);
        EventsManager::on(F("wifi_ip_set"), &WatchService::wifiStateGotIP);
        EventsManager::on(F("wifi_disconnect"), &WatchService::wifiStateDisconnected);
        EventsManager::on(F("rtc_set"), &WatchService::rtcReady);
        EventsManager::on(F("rtc_interrupt"), &WatchService::rtcInterrupt);
        EventsManager::on(F("cpu_freq_mhz"), &WatchService::cpuFrequencyChange);
        EventsManager::on(F("power_state"), &WatchService::powerStateChange);
    };

    void setup() override {
        _debugger.Debug(_component, "setup begin");
        _led->setBrightness(0);
        _frameBuffer->getDisplay()->setBacklight(255);
        _frameBuffer->getDisplay()->setEnabled(true);

        // Set the entire screen to solid black
        _frameBuffer->setAll(FB_BLACK);

        sDOS_FrameBuffer::Coordinate topLeft = sDOS_FrameBuffer::Coordinate(10,10);
        sDOS_FrameBuffer::Coordinate topRight = sDOS_FrameBuffer::Coordinate(DISPLAY_HEIGHT - 10, 10);
        sDOS_FrameBuffer::Coordinate bottomLeft = sDOS_FrameBuffer::Coordinate(10, DISPLAY_WIDTH - 10);
        sDOS_FrameBuffer::Coordinate bottomRight = sDOS_FrameBuffer::Coordinate(DISPLAY_HEIGHT - 10, DISPLAY_WIDTH - 10);

        sDOS_FrameBuffer::Coordinate innerTopLeft = topLeft.move(10,10);
        sDOS_FrameBuffer::Coordinate innerTopRight = topRight.move(-10,10);
        sDOS_FrameBuffer::Coordinate innerBottomLeft = bottomLeft.move(10,-10);
        sDOS_FrameBuffer::Coordinate innerBottomRight = bottomRight.move(-10,-10);

        // Outer white frame border
        _frameBuffer->drawLine(0,0,0,DISPLAY_WIDTH-1, FB_WHITE);
        _frameBuffer->drawLine(DISPLAY_HEIGHT-1 ,0,0,0, FB_WHITE);
        _frameBuffer->drawLine(DISPLAY_HEIGHT-1, DISPLAY_WIDTH-1, 0,DISPLAY_WIDTH-1, FB_WHITE);
        _frameBuffer->drawLine(DISPLAY_HEIGHT-1 ,0,DISPLAY_HEIGHT-1,DISPLAY_WIDTH-1, FB_WHITE);

        // Inner colour square
        _frameBuffer->drawLine(topLeft, topRight, FB_RED);
        _frameBuffer->drawLine(topRight, bottomRight, FB_GREEN);
        _frameBuffer->drawLine(bottomRight, bottomLeft, FB_BLUE);
        _frameBuffer->drawLine(bottomLeft, topLeft, FB_PINK);
        _frameBuffer->drawLine(topLeft, bottomRight, FB_YELLOW);
        _frameBuffer->drawLine(bottomLeft, topRight, FB_CYAN);

        // Inner-most colour square
        _frameBuffer->drawLine(innerTopLeft, innerTopRight, FB_RED);
        _frameBuffer->drawLine(innerTopRight, innerBottomRight, FB_GREEN);
        _frameBuffer->drawLine(innerBottomRight, innerBottomLeft, FB_BLUE);
        _frameBuffer->drawLine(innerBottomLeft, innerTopLeft, FB_PINK);

        // All done
        _debugger.Debug(_component, "setup over");
    };

    void loop() override {
        if (WatchService::_faciaButtonPressed) {
            WatchService::_faciaButtonPressed = false;
            touch();
        }
        if(WatchService::_wifiStateHasChanged){
            WatchService::_wifiStateHasChanged = false;
            _debugger.Debug(_component, "Wifi State has changed ¯\\_(ツ)_/¯");
        }
        if(WatchService::_rtcReadyFired){
            WatchService::_rtcReadyFired = false;
            _debugger.Debug(_component, "RTC ready ¯\\_(ツ)_/¯");
            _rtc->setAlarmInMinutes(1);
        }
        if(WatchService::_rtcInterruptFired){
            WatchService::_rtcInterruptFired = false;
            _debugger.Debug(_component, "RTC interrupt ¯\\_(ツ)_/¯");
            _rtc->setAlarmInMinutes(1);
        }
        if(WatchService::_cpuFrequencyMhzInterruptFired){
            WatchService::_cpuFrequencyMhzInterruptFired = false;
            _debugger.Debug(_component, "CPU Frequency now %d", WatchService::_cpuFreqencyMhz);
        }
        if(WatchService::_chargeStateChanged){
            WatchService::_chargeStateChanged = false;
            if(WatchService::_isOnCharge){
                if(!_hasActiveWifiRequest){
                    _wifi->addRequestActive();
                    _ota->activate();
                    //BluetoothManager::addRequest();
                    _hasActiveWifiRequest = true;
                }
                _debugger.Debug(_component, "I have been put on charge");

            }else{
                _debugger.Debug(_component, "I have been taken off charge");
                if(_hasActiveWifiRequest){
                    _ota->deactivate();
                    _wifi->removeRequestActive();
                    //BluetoothManager::removeRequest();
                    _hasActiveWifiRequest = false;
                }
            }
        }
    };

    void touch() {
        if (_bleh) {
            _bleh = false;
            _frameBuffer->getDisplay()->setBacklight(255);
            _frameBuffer->getDisplay()->setEnabled(true);
            _frameBuffer->setAll(255, 0, 0);
            _led->setBrightness(0);
        } else {
            _bleh = true;
            _frameBuffer->getDisplay()->setBacklight(255);
            _frameBuffer->getDisplay()->setEnabled(true);
            _frameBuffer->setAll(0, 255, 0);
            _led->setBrightness(255);
        }
        for(uint i = 0; i <= rand()%15 + 5; i++){
            _frameBuffer->drawLine(
                    rand() % DISPLAY_HEIGHT, rand() % DISPLAY_WIDTH,
                    rand() % DISPLAY_HEIGHT, rand() % DISPLAY_WIDTH,
                    rand() % 155 + 100, rand() % 155 + 100, rand() % 155 + 100
            );
        }
    };

    String getName() override { return _component; };

    bool isActive() override { return true; };

    static void faciaButtonPressCallback(const String& payload) {
        WatchService::_faciaButtonPressed = true;
    };

    static void wifiStateChangeOn(const String& payload){
        WatchService::_wifiIsOn = true;
        WatchService::_wifiStateHasChanged = true;
    }
    static void wifiStateChangeOff(const String& payload){
        WatchService::_wifiIsOn = false;
        WatchService::_wifiStateHasChanged = true;
    }
    static void wifiStateGotIP(const String& payload){
        WatchService::_wifiIp = payload;
        WatchService::_wifiStateHasChanged = true;
    }
    static void wifiStateDisconnected(const String& payload){
        WatchService::_wifiIp.clear();
        WatchService::_wifiStateHasChanged = true;
    }
    static void rtcReady(const String& payload){
        WatchService::_rtcReadyFired = true;
    }
    static void rtcInterrupt(const String& payload){
        WatchService::_rtcInterruptFired = true;
    }
    static void cpuFrequencyChange(const String & payload){
        WatchService::_cpuFreqencyMhz = strtol(payload.c_str(), nullptr, 10);
        WatchService::_cpuFrequencyMhzInterruptFired = true;
    }
    static void powerStateChange(const String & payload){
        WatchService::_chargeStateChanged = true;
        WatchService::_isOnCharge = payload == "charging" ? true : false;
    }

private:
    String _component = "timepiece";
    Debugger _debugger;
    EventsManager _eventsManager;
    sDOS_PCF8563 *_rtc;
    WiFiManager *_wifi;
    BluetoothManager *_bt;
    sDOS_OTA_Service *_ota;
    sDOS_CPU_SCALER *_cpuScaler;
    sDOS_FrameBuffer *_frameBuffer;
    sDOS_TTP223 *_button;
    sDOS_LED_MONO *_led;
    bool _bleh = false;
    static bool _faciaButtonPressed;
    static bool _wifiStateHasChanged;
    static bool _wifiIsOn;
    static String _wifiIp;
    static bool _rtcReadyFired;
    static bool _rtcInterruptFired;
    static unsigned int _cpuFreqencyMhz;
    static bool _cpuFrequencyMhzInterruptFired;
    static bool _chargeStateChanged;
    static bool _isOnCharge;
    bool _hasActiveWifiRequest = false;
};

bool WatchService::_faciaButtonPressed = false;
bool WatchService::_wifiStateHasChanged = false;
bool WatchService::_wifiIsOn = false;
String WatchService::_wifiIp = "";
bool WatchService::_rtcReadyFired = false;
bool WatchService::_rtcInterruptFired = false;
bool WatchService::_cpuFrequencyMhzInterruptFired = false;
unsigned int WatchService::_cpuFreqencyMhz = 0;
bool WatchService::_chargeStateChanged = false;
bool WatchService::_isOnCharge = false;