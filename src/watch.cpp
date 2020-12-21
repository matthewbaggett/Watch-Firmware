#include "watch.hpp"

//#define ENABLE_SERVICE_NTP

Watch _timepiece;

void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.setDebugOutput(true);
    delay(300);
}

void loop() {
    //Serial.println("Loop!");
    _timepiece.Loop();
}