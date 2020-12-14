#include "watch.hpp"

//#define ENABLE_SERVICE_NTP

Watch _timepiece;

void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.setDebugOutput(true);
    delay(300);
    Serial.println("Alive?");
}

void loop() {
    Serial.println("Loop!");
    //_timepiece.Loop();
}