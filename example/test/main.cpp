#include "M5StickCPlus.h"
#include "soundOut.hpp"

void setup()
{
  M5.begin();
  sOut.begin(2, 100);
  randomSeed(analogRead(0));
}

void loop()
{
  if (M5.BtnA.wasPressed())
    sOut.NumberOut(int(random(0, 1000)));
  if (M5.BtnB.wasPressed()) {
    uint8_t s;
    if ((s = sOut.getSpeed()) < 120)
      sOut.setSpeed(s+5);
    else
      sOut.setSpeed(10);
  }
  M5.update();
}
