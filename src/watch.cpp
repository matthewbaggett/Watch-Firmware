#include "watch.hpp"

#define ENABLE_SERVICE_NTP

Watch _timepiece;

void setup() {
    _timepiece.Setup();
}

void loop() {
    _timepiece.Loop();
}