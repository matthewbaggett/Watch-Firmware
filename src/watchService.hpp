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

class WatchService : public sDOS_Abstract_Service
{
    public:
        WatchService(Debugger &debugger, EventsManager &eventsManager, AbstractRTC *rtc, WiFiManager *wifi, BluetoothManager *bt, sDOS_CPU_SCALER *cpuScaler, sDOS_FrameBuffer *frameBuffer) 
            : _debugger(debugger), _eventsManager(eventsManager), _rtc(rtc), _wifi(wifi), _bt(bt), _cpuScaler(cpuScaler), _frameBuffer(frameBuffer){};
        
        void setup(){
            _debugger.Debug(_component, "setup begin");
            uint8_t r=0, g=0, b=0;
            _frameBuffer->setAll(r, g, b);
            _debugger.Debug(_component, "setup over");
        };

        void loop(){
            if(_bleh){
                _bleh = false;
                _frameBuffer->setAll(255, 0, 0);
                //_frameBuffer->drawLine(1, 1, DISPLAY_WIDTH-2, DISPLAY_HEIGHT-2, 0, 255, 0);
            }else{
                _bleh = true;
                uint8_t r=0, g=0, b=0;
                _frameBuffer->setAll(r, g, b);
            }
        };

        String getName(){ return _component; };

        bool isActive() { return true; };
        
    private:
        String _component = "timepiece";
        Debugger _debugger;
        EventsManager _eventsManager;
        AbstractRTC *_rtc;
        WiFiManager *_wifi;
        BluetoothManager *_bt;
        sDOS_CPU_SCALER *_cpuScaler;
        sDOS_FrameBuffer *_frameBuffer;
        bool _bleh = false;
};
