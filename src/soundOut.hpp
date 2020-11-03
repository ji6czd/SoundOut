#ifndef _SOUNDOUT_HPP_
#define _SOUNDOUT_HPP_
#include <cstdint>
#include "Arduino.h"
#include "MorseTable.h"

struct beepCmd {
  uint8_t type;
  uint16_t len;
  uint16_t freq;
};

class SoundOut {
public:
  int begin(uint8_t pin, uint8_t volume);
  void beepOut(uint16_t freq, uint16_t length);
  void morseOut(const char* s);
  void numberOut(uint16_t num);
  uint16_t getSpeed() {return cpm; };
  uint16_t setSpeed(int speed) { return cpm = speed; };
  bool isProgress() { return progress; };
  bool stop() { return progress = false; }; 
private:
  static QueueHandle_t xBeepCmdQueue;
  static SemaphoreHandle_t xBeepMutex;;
  static String sSharedOutStr;
  static uint8_t cpm;
  static uint8_t vol;
  static bool progress;
  static void beepFunc(uint16_t freq, uint16_t length);
  static void morseLong();
  static void morseShort();
  static void morseSpace();
  static char morseFunc(char c);
  static uint16_t numberFunc(uint16_t n);
  static void beepTask(void* arg);
};
extern SoundOut SOut;

#endif
