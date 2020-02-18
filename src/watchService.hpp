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
#include <stdlib.h>

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
        _led->setBrightness(0);
        _frameBuffer->getDisplay()->setBacklight(255);
        _frameBuffer->getDisplay()->setEnabled(true);

        // Set the entire screen to solid black
        _frameBuffer->setAll(0, 0, 0);

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

        _eventsManager.on(F("TTP223_down"), &WatchService::faciaButtonPressCallback);
        _debugger.Debug(_component, "setup over");
    };

    void loop() {
        if (WatchService::_faciaButtonPressed) {
            WatchService::_faciaButtonPressed = false;
            touch();
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

    String getName() { return _component; };

    bool isActive() { return true; };

    static void faciaButtonPressCallback(String payload) {
        WatchService::_faciaButtonPressed = true;
    };

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
    static bool _faciaButtonPressed;
};

bool WatchService::_faciaButtonPressed = false;
