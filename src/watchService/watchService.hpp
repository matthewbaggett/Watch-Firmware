#include "kern_inc.h"
#include "abstracts/service.hpp"
#include <NTPClient.h>
#include <debugger.hpp>
#include <events.hpp>
#include <drivers/rtc/rtc.hpp>
#include <drivers/touch/button.hpp>
#include <drivers/WiFiManager.hpp>
#include <drivers/BluetoothManager.hpp>
#include <services/cpu_scaler.hpp>
#include <services/ntp.hpp>
#include <services/ota.hpp>
#include <services/sleeptune.hpp>
#include <drivers/display/frame_buffer.hpp>
#include <drivers/display/fonts/FreeMonoBold24pt7b.h>
#include <drivers/display/fonts/Picopixel.h>
//#include <icons/BATTERY_PERCENT_0.xbm.h>
//#include <icons/BATTERY_PERCENT_25.xbm.h>
//#include <icons/BATTERY_PERCENT_50.xbm.h>
//#include <icons/BATTERY_PERCENT_75.xbm.h>
//#include <icons/BATTERY_PERCENT_100.xbm.h>
//#include <icons/BATTERY_PERCENT_CHARGING.xbm.h>
#include <cstdlib>
#include <string>

#ifndef NTP_UPDATE_INTERVAL_SECONDS
#define NTP_UPDATE_INTERVAL_SECONDS 60 * 60 * 12
#endif

class WatchService : public sDOS_Abstract_Service {
public:
    WatchService(
        Debugger * debugger,
        EventsManager * eventsManager,
        sDOS_FrameBuffer * frameBuffer,
        sDOS_RTC *rtc,
        WiFiManager *wifi,
        BluetoothManager *bt,
        sDOS_CPU_SCALER *cpuScaler,
        sDOS_OTA_Service *ota
    ) : sDOS_Abstract_Service(debugger, eventsManager),
        _frameBuffer(frameBuffer),
        _rtc(rtc),
        _wifi(wifi),
        _bt(bt),
        _cpuScaler(cpuScaler),
        _ota(ota)
    {
        Serial.println("Entered WatchService");
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

        //debugger->Debug(_component, "setup begin");
        //paintTime();
        //debugger->Debug(_component, "paintTime");
        //paintTray();
        //debugger->Debug(_component, "paintTray");

        // All done
        //debugger->Debug(_component, "setup over");

        // Allow Debugging with bluetooth UART
        //BluetoothManager::addRequest();
    };

    void paintTime() {
        this->_debugger->Debug(_component, "paintTime() entry");
        sDOS_FrameBuffer::Colour background = FB_BLACK;
        sDOS_FrameBuffer::Colour colour = FB_WHITE;
        // Set the entire screen to solid black
        _frameBuffer->fillEntireFrame(background);
        GFXfont timeFont = FreeMonoBold24pt7b;

        this->_debugger->Debug(_component, "paintTime() B");

        // Hour and minute as string
        String hour = String(this->_rtc->getTime().hour());
        this->_debugger->Debug(_component, "paintTime() B2");

        String minute = String(this->_rtc->getTime().minute());
        this->_debugger->Debug(_component, "paintTime() B3");

        // left pad them with zeros if needed
        this->_debugger->Debug(_component, "paintTime() B4");
        if(hour.length() < 2) hour = "0" + hour;
        this->_debugger->Debug(_component, "paintTime() B5");
        if(minute.length() < 2) minute = "0" + minute;

        this->_debugger->Debug(_component, "paintTime() C");

        // Work out how big the text is gonna be
        sDOS_FrameBuffer::Region affectedRegionHour   = _frameBuffer->boundText(0, 0, hour, &timeFont);
        sDOS_FrameBuffer::Region affectedRegionMinute = _frameBuffer->boundText(0, 0, minute, &timeFont);

        this->_debugger->Debug(_component, "paintTime() D");

        // Offset calculation
        uint16_t leftPadHour = (_frameBuffer->getHeight() - affectedRegionHour.getWidth()) / 2;
        uint16_t leftPadMinutes = (_frameBuffer->getHeight() - affectedRegionMinute.getWidth()) / 2;
        uint16_t topPadPerLineAdvance = timeFont.yAdvance - 5;
        uint16_t topPad = (_frameBuffer->getWidth() - affectedRegionHour.getHeight()) / 2;
        topPad = topPad - (topPadPerLineAdvance / 2);

        this->_debugger->Debug(_component, "paintTime() E");

        // Manually jiggle it over s'more because it doesn't look quite right
        leftPadHour -= 3;
        leftPadMinutes -= 3;
        topPad -= 18;

        //this->_debugger->Debug(_component, "TopPadding : FB Width : %d, region height: %d, topPad : %d", _frameBuffer->getWidth(), affectedRegionHour.getHeight(), topPad);
        //this->_debugger->Debug(_component, "LeftPadding: FB Height: %d, region width : %d, leftPad: %d", _frameBuffer->getHeight(), affectedRegionHour.getWidth(), leftPadHour);

        this->_debugger->Debug(_component, "paintTime() F");

        affectedRegionHour = _frameBuffer->drawText(leftPadHour, topPad, hour, &timeFont, colour);
        affectedRegionMinute = _frameBuffer->drawText(leftPadMinutes, topPad + topPadPerLineAdvance, minute, &timeFont, colour);
        //affectedRegionHour.highlight(_frameBuffer, FB_BLUE);
        //affectedRegionMinute.highlight(_frameBuffer, FB_PINK);

        this->_debugger->Debug(_component, "paintTime() G");

        //drawDebugBoundingBox();
        this->_debugger->Debug(_component, "paintTime() exit");
        paintTray();
    }

    void paintTray() {
        this->_debugger->Debug(_component, "paintTray() entry");
        const sDOS_FrameBuffer::Colour background = FB_BLACK;
        const sDOS_FrameBuffer::Colour colour = FB_WHITE;
        const GFXfont * trayFont = &Picopixel;
        uint16_t trayHeight = 10;
        sDOS_FrameBuffer::Coordinate topLeft     = sDOS_FrameBuffer::Coordinate(0,12);
        sDOS_FrameBuffer::Coordinate topRight    = sDOS_FrameBuffer::Coordinate(_frameBuffer->getHeight() - 1, 12);
        _frameBuffer->drawLine(topLeft, topRight, colour);
        if(this->_isOnCharge) {
            // _frameBuffer->drawXBM(_frameBuffer->getHeight() - 30, 0, BATTERY_PERCENT_CHARGING_width, BATTERY_PERCENT_CHARGING_height, BATTERY_PERCENT_CHARGING_bits);
        } else {
            // _frameBuffer->drawXBM(_frameBuffer->getHeight() - 30, 0, BATTERY_PERCENT_0_width, BATTERY_PERCENT_0_height, BATTERY_PERCENT_0_bits);
        }
        this->_debugger->Debug(_component, "paintTime() exit");

    }

    void loop() override {
        if (WatchService::_faciaButtonPressed) {
            WatchService::_faciaButtonPressed = false;
            touch();
        }
        if(WatchService::_wifiStateHasChanged) {
            WatchService::_wifiStateHasChanged = false;
            this->_debugger->Debug(_component, "Wifi State has changed ¯\\_(ツ)_/¯");
        }
        if(WatchService::_rtcReadyFired) {
            WatchService::_rtcReadyFired = false;
            //this->_debugger->Debug(_component, "RTC ready ¯\\_(ツ)_/¯");
            this->_rtc->setAlarmInMinutes(1);
            paintTime();
        }
        if(WatchService::_rtcInterruptFired) {
            WatchService::_rtcInterruptFired = false;
            // this->_debugger->Debug(_component, "RTC interrupt ¯\\_(ツ)_/¯");
            this->_rtc->setAlarmInMinutes(1);
            paintTime();
        }
        if(WatchService::_cpuFrequencyMhzInterruptFired) {
            WatchService::_cpuFrequencyMhzInterruptFired = false;
        }
        if(WatchService::_chargeStateChanged) {
            WatchService::_chargeStateChanged = false;
            paintTray();
            if(WatchService::_isOnCharge) {
                this->_debugger->Debug(_component, "I have been put on charge");
                if(!_hasActiveRadioRequests) {
                    //_wifi->addRequestActive();
                    //_ota->activate();
                    //BluetoothManager::addRequest();
                    _hasActiveRadioRequests = true;
                }
            } else {
                this->_debugger->Debug(_component, "I have been taken off charge");
                if(_hasActiveRadioRequests) {
                    //_ota->deactivate();
                    //_wifi->removeRequestActive();
                    //BluetoothManager::removeRequest();
                    _hasActiveRadioRequests = false;
                }
            }
        }
    };

    void setup() {
        Serial.println("WatchService::setup()");
    };

    void touch() {
        if (_bleh) {
            _bleh = false;
            _frameBuffer->getDisplay()->setBacklight(255);
            _frameBuffer->getDisplay()->setEnabled(true);
            _frameBuffer->fillEntireFrame(FB_RED);
            _led->setBrightness(0);
            _frameBuffer->drawText(10, 10, "One Side", &Picopixel, FB_GREEN);
        } else {
            _bleh = true;
            _frameBuffer->getDisplay()->setBacklight(255);
            _frameBuffer->getDisplay()->setEnabled(true);
            _frameBuffer->fillEntireFrame(FB_GREEN);
            _led->setBrightness(255);
            _frameBuffer->drawText(10, 10, "And the other", &Picopixel, FB_RED);
        }

        for(uint i = 0; i <= rand()%15 + 5; i++) {
            sDOS_FrameBuffer::Colour colour = sDOS_FrameBuffer::Colour(rand() % 155 + 100,rand() % 155 + 100,rand() % 155 + 100);

            _frameBuffer->drawLine(
                rand() % _frameBuffer->getHeight(), rand() % _frameBuffer->getWidth(),
                rand() % _frameBuffer->getHeight(), rand() % _frameBuffer->getWidth(),
                colour
            );
        }
    };

    String getName() override {
        return _component;
    };

    bool isActive() override {
        return true;
    };

    static void faciaButtonPressCallback(const String& payload) {
        WatchService::_faciaButtonPressed = true;
    };

    static void wifiStateChangeOn(const String& payload) {
        WatchService::_wifiStateHasChanged = true;
    }
    static void wifiStateChangeOff(const String& payload) {
        WatchService::_wifiStateHasChanged = true;
    }
    static void wifiStateGotIP(const String& payload) {
        WatchService::_wifiIp = payload;
        WatchService::_wifiStateHasChanged = true;
    }
    static void wifiStateDisconnected(const String& payload) {
        WatchService::_wifiIp.clear();
        WatchService::_wifiStateHasChanged = true;
    }
    static void rtcReady(const String& payload) {
        WatchService::_rtcReadyFired = true;
    }
    static void rtcInterrupt(const String& payload) {
        WatchService::_rtcInterruptFired = true;
    }
    static void cpuFrequencyChange(const String & payload) {
        WatchService::_cpuFreqencyMhz = strtol(payload.c_str(), nullptr, 10);
        WatchService::_cpuFrequencyMhzInterruptFired = true;
    }
    static void powerStateChange(const String & payload) {
        WatchService::_chargeStateChanged = true;
        WatchService::_isOnCharge = payload == "charging";
    }

    void drawDebugBoundingBox() {
        sDOS_FrameBuffer::Coordinate topLeft     = sDOS_FrameBuffer::Coordinate(0,0);
        sDOS_FrameBuffer::Coordinate topRight    = sDOS_FrameBuffer::Coordinate(_frameBuffer->getHeight() - 1, 0);
        sDOS_FrameBuffer::Coordinate bottomLeft  = sDOS_FrameBuffer::Coordinate(0, _frameBuffer->getWidth() - 1);
        sDOS_FrameBuffer::Coordinate bottomRight = sDOS_FrameBuffer::Coordinate(_frameBuffer->getHeight() - 1, _frameBuffer->getWidth() - 1);

        _frameBuffer->drawLine(topLeft,    topRight,    FB_DARK_GREY);
        _frameBuffer->drawLine(bottomLeft, bottomRight, FB_DARK_GREY);
        _frameBuffer->drawLine(topLeft,    bottomLeft,  FB_DARK_GREY);
        _frameBuffer->drawLine(topRight,   bottomRight, FB_DARK_GREY);

        _frameBuffer->drawLine(_frameBuffer->getHeight() / 2, 0,_frameBuffer->getHeight() / 2, _frameBuffer->getWidth() - 1, FB_DARK_GREY);
        _frameBuffer->drawLine(0,_frameBuffer->getWidth() / 2,_frameBuffer->getHeight() - 1,_frameBuffer->getWidth() / 2, FB_DARK_GREY);
    }

    void drawTestBoxes() {
        _frameBuffer->fillEntireFrame(FB_BLACK);

        sDOS_FrameBuffer::Coordinate topLeft = sDOS_FrameBuffer::Coordinate(10,10);
        sDOS_FrameBuffer::Coordinate topRight = sDOS_FrameBuffer::Coordinate(_frameBuffer->getHeight() - 10, 10);
        sDOS_FrameBuffer::Coordinate bottomLeft = sDOS_FrameBuffer::Coordinate(10, _frameBuffer->getWidth() - 10);
        sDOS_FrameBuffer::Coordinate bottomRight = sDOS_FrameBuffer::Coordinate(_frameBuffer->getHeight() - 10, _frameBuffer->getWidth() - 10);

        sDOS_FrameBuffer::Coordinate innerTopLeft = topLeft.move(10,10);
        sDOS_FrameBuffer::Coordinate innerTopRight = topRight.move(-10,10);
        sDOS_FrameBuffer::Coordinate innerBottomLeft = bottomLeft.move(10,-10);
        sDOS_FrameBuffer::Coordinate innerBottomRight = bottomRight.move(-10,-10);

        // Outer white frame border
        _frameBuffer->drawLine(0,0,0,_frameBuffer->getWidth()-1, FB_WHITE);
        _frameBuffer->drawLine(_frameBuffer->getHeight()-1,0,0,0, FB_WHITE);
        _frameBuffer->drawLine(_frameBuffer->getHeight()-1, _frameBuffer->getWidth()-1, 0,_frameBuffer->getWidth()-1, FB_WHITE);
        _frameBuffer->drawLine(_frameBuffer->getHeight()-1,0,_frameBuffer->getHeight()-1,_frameBuffer->getWidth()-1, FB_WHITE);

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
    }


protected:
    sDOS_FrameBuffer *_frameBuffer;
    String _component = "timepiece";
    sDOS_RTC *_rtc;
    WiFiManager *_wifi;
    BluetoothManager *_bt;
    sDOS_CPU_SCALER *_cpuScaler;
    sDOS_OTA_Service *_ota;
    sDOS_BUTTON *_button;
    sDOS_LED_MONO *_led;
    bool _bleh = false;
    static bool _faciaButtonPressed;
    static bool _wifiStateHasChanged;
    static String _wifiIp;
    static bool _rtcReadyFired;
    static bool _rtcInterruptFired;
    static unsigned int _cpuFreqencyMhz;
    static bool _cpuFrequencyMhzInterruptFired;
    static bool _chargeStateChanged;
    static bool _isOnCharge;
    bool _hasActiveRadioRequests = false;
};

bool WatchService::_faciaButtonPressed = false;
bool WatchService::_wifiStateHasChanged = false;
String WatchService::_wifiIp = "";
bool WatchService::_rtcReadyFired = false;
bool WatchService::_rtcInterruptFired = false;
bool WatchService::_cpuFrequencyMhzInterruptFired = false;
unsigned int WatchService::_cpuFreqencyMhz = 0;
bool WatchService::_chargeStateChanged = false;
bool WatchService::_isOnCharge = false;