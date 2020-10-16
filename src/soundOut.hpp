#ifndef _SOUNDOUT_HPP_
#define _SOUNDOUT_HPP_
#include <cstdint>

using namespace std;
enum soundOut_Mode_t {
  length,
  pitch
};
class soundOut {
public:
  void begin(uint8_t outPin, uint8_t outVol);
  static void beep(int f, int m);
  static void waitBeep(int f, int m);
  static void morseOut(const char c);
  static void NumberOut(int num);
  static void speakOut(const char* s);
  static void stop();
  static void setMode(soundOut_Mode_t mode) { oMode = mode; };
  static soundOut_Mode_t getMode() { return oMode; };
  static void setSpeed(uint8_t speed) { cpm = speed; };
  static uint8_t getSpeed() { return cpm; };
private:
  const static int resolution = 8;
  static int freq;
  static int mSec;
  static int vol;
  static void beepFunc();
  static void morseLong();
  static void morseShort();
  static void morseSpace();
  static void lowTone();
  static void highTone();
  static unsigned int cpm;
  static unsigned int tone;
  static soundOut_Mode_t oMode;
	void static DAC_Create();
	static void DAC_Release();
	static int DAC_Write(int len, int16_t *wav);
};
extern soundOut sOut;

#endif
