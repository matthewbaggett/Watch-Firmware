#include "watch.hpp"

Watch _timepiece;

void setup()
{
    _timepiece.Setup();
}

void loop()
{
   _timepiece.Loop();
}