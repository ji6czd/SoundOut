#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Arduino.h"
#include "soundOut.hpp"

#include <thread>
#include <cstdint>
#include "MorseTable.h"

using namespace std;
int soundOut::freq, soundOut::mSec, soundOut::vol;
soundOut_Mode_t soundOut::oMode;
unsigned int soundOut::tone=1500;
unsigned int soundOut::cpm=10;

soundOut sOut;

void soundOut::beep(int f, int m)
{
  freq=f;
  mSec=m;
  thread th(beepFunc);
  th.detach();
}

void soundOut::waitBeep(int f, int m)
{
  freq=f;
  mSec=m;
  thread th(beepFunc);
  th.join();
}

void soundOut::beepFunc()
{
	ledcWriteTone(0, freq);
	ledcWrite(0, vol);
	delay(mSec);
	ledcWriteTone(0, 0);
}

void soundOut::morseOut(const char c)
{
	if (isspace(c)) {
		morseSpace();
		}
	else if (isalnum(c)) {
		unsigned char s = MorseTable[toupper(c)-0x30];
		while (s != 1) {
			if (s & 0b1) {
				morseLong();
			} else {
				morseShort();
			}
			s >>= 1;
			}
	}
	morseSpace();
}

void soundOut::NumberOut(int num)
{
  int i=100000000;
  for (i; (num && !(num / i)); i/=10)
    ; // 空のループ
  for (i; i > 0; i /= 10) {
    unsigned char s; // 符号テーブル配列の添字
    int div;
    div = num / i;
    s = NumberTable[div];
    while (s != 1) {
      if (s & 0b1) {
	if (oMode == length) morseLong();
	else highTone();
      } else {
	if (oMode == length) morseShort();
	else lowTone();
      }
      s >>= 1;
    }
    morseSpace();
    num -= div*i;
  }
}

void soundOut::morseShort() {
	waitBeep(tone, 1000/(cpm/2));
	delay(1000/(cpm/2));
}

void soundOut::morseLong() {
	waitBeep(tone, 1000/(cpm/2)*3);
	delay(1000/(cpm/2));
}

void soundOut::morseSpace()
{
	delay(1000/(cpm/2)*3);
}

void soundOut::highTone()
{
  waitBeep(2250, 1000/(cpm/2));
  delay(1000/(cpm/2));
}

void soundOut::lowTone()
{
  waitBeep(1500, 1000/(cpm/2));
  delay(1000/(cpm/2));
}

void soundOut::begin(uint8_t outPin, uint8_t outVol)
{
	ledcSetup(0, 1000, 13);
	ledcAttachPin(outPin, 0);
	vol=outVol;
}
