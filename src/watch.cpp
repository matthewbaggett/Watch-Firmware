#include "watch.hpp"

#define ENABLE_SERVICE_NTP

Watch _timepiece;

void setup() {}

void loop() {
    _timepiece.Loop();
}