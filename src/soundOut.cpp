#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Arduino.h"
#include "soundOut.hpp"

SoundOut SOut;
QueueHandle_t SoundOut::xBeepCmdQueue;
SemaphoreHandle_t SoundOut::xBeepMutex;;
String SoundOut::sSharedOutStr;
uint8_t SoundOut::cpm;
uint8_t SoundOut::vol;
bool SoundOut::progress;
const uint16_t morseTone=1500;
const TickType_t xTicksToWait = 10U; // [ms]

void SoundOut::beepFunc(uint16_t freq, uint16_t length)
{
  ledcWriteTone(0, freq);
  ledcWrite(0, vol);
  while (length / 100) {
    delay(100);
    if (!progress)  {
      length=0;
      break;
    }
    length -= 100;
  }
  if (length) delay(length);
  ledcWriteTone(0, 0);
}

void SoundOut::morseShort() {
  beepFunc(morseTone, 1000/(cpm/2));
  delay(1000/(cpm/2));
}

void SoundOut::morseLong() {
  beepFunc(morseTone, 1000/(cpm/2)*3);
  delay(1000/(cpm/2));
}

void SoundOut::morseSpace()
{
  delay(1000/(cpm/2)*3);
}

char SoundOut::morseFunc(char c)
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
  return c;
}

uint16_t SoundOut::numberFunc(uint16_t num)
{
  uint16_t i=10000;
  for (i; (num && !(num / i)); i/=10)
    ; // 空のループ
  if (num == 0) i = 1;
  for (i; i > 0; i /= 10) {
    unsigned char s; // 符号テーブル配列の添字
    int div;
    div = num / i;
    s = NumberTable[div];
    while (s != 1) {
      if (!progress) return 0;
      if (s & 0b1) {
	morseLong();
      } else {
	morseShort();
      }
      s >>= 1;
    }
    morseSpace();
    num -= div*i;
  }
  return num;
}

void SoundOut::beepTask(void* arg)
{
  BaseType_t xStatus;
  beepCmd cmd;
  xSemaphoreGive(xBeepMutex);
  while(1) {
    xStatus = xSemaphoreTake(xBeepMutex, xTicksToWait);
    if (xStatus != pdTRUE) {
      xSemaphoreGive(xBeepMutex);
       break;
    }
    xStatus = xQueueReceive(xBeepCmdQueue, &cmd, xTicksToWait);
    if(xStatus == pdPASS) {
      progress = true;
      if (cmd.freq == 0) {
	// ビープ以外のコマンド
	if (sSharedOutStr.length()) {
	  uint16_t i=0;
	  char c;
	  while ((c = sSharedOutStr.charAt(i)) && progress) {
	    morseFunc(c);
	    i++;
	  }
	  sSharedOutStr = "";
	  xSemaphoreGive(xBeepMutex);
	}
	else {
	  numberFunc(cmd.len);
	  xSemaphoreGive(xBeepMutex);
	}
      }
      else {
	beepFunc(cmd.freq, cmd.len);
	xSemaphoreGive(xBeepMutex);
      }
    } else {
      xSemaphoreGive(xBeepMutex);
    }
    progress = false;
  }
}

void SoundOut::morseOut(const char* str)
{
  BaseType_t xStatus = xSemaphoreTake(xBeepMutex, xTicksToWait);
  if (xStatus != pdTRUE) {
    xSemaphoreGive(xBeepMutex);
  }
  if (!sSharedOutStr.length()) {
    sSharedOutStr = str;
  }
  beepCmd cmd;
  cmd.type=2; // string morse
  cmd.freq=0;
  cmd.len=0;
  xStatus = xQueueSend(xBeepCmdQueue, &cmd, 0);
  xSemaphoreGive(xBeepMutex);
}

void SoundOut::beepOut(uint16_t freq, uint16_t len)
{
  BaseType_t xStatus;
  beepCmd cmd;
  cmd.type=0; // Beep
  cmd.freq=freq;
  cmd.len=len;
  progress=true;
  xStatus = xQueueSend(xBeepCmdQueue, &cmd, 0);
  return;
}

void SoundOut::numberOut(uint16_t num)
{
  BaseType_t xStatus;
  beepCmd cmd;
  cmd.type=1; // number
  cmd.freq=0;
  cmd.len=num;
  xStatus = xQueueSend(xBeepCmdQueue, &cmd, 0);
  return;
}

int SoundOut::begin(uint8_t pin, uint8_t volume)
{
  ledcWriteTone(0, 0);
  ledcAttachPin(pin, 0);
  xBeepCmdQueue = xQueueCreate(8, sizeof(beepCmd));
  xBeepMutex = xSemaphoreCreateMutex();
  if(xBeepMutex != NULL) {
    sSharedOutStr = "";
    cpm=40;
    vol=volume;
    xTaskCreate(beepTask, "beepTask", 4096, NULL, 2, NULL);
  } else {
    return -1;
  }
  beepOut(2000, 100);
  beepOut(1000, 100);
  return 0;
}
