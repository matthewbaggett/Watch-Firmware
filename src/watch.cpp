#define XSTR(x) STR(x)
#define STR(x) #x
#include "watch.hpp"

//#define ENABLE_SERVICE_NTP

Watch _timepiece;

void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.setDebugOutput(true);
    delay(1000);
    Serial.flush();
    _timepiece.setup();
}

void loop() {
    _timepiece.Loop();
}