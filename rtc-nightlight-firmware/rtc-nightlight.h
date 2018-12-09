// ***************************************************************************
// RTC based automatic LED lamp - PIC16F883 Firmware.
//
// Copyright (c) 2018 Dilshan R Jayakody.
//
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
// THE SOFTWARE.
// ***************************************************************************

#ifndef RTC_NIGHTLIGHT_MAIN_HEADER
#define RTC_NIGHTLIGHT_MAIN_HEADER

#define SUCCESS 0x00
#define FAIL 0xFF

const code char helpText1[] = "\r\nSystem configuration";
const code char helpText1A[] = "\r\n====================\r\n";
const code char helpText2[] = "\r\n 1.) Set system time";
const code char helpText3[] = "\r\n 2.) Set start time";
const code char helpText4[] = "\r\n 3.) Set stop time";
const code char helpText5[] = "\r\n 4.) Show configuration";
const code char helpText6[] = "\r\n 5.) Version";
const code char helpText7[] = "\r\n 6.) Exit";
const code char helpText8[] = "\r\n\r\nSelection > ";

const code char intoHelp1[] = "\r\nPress [SPACE] or [ENTER]";
const code char intoHelp2[] = " to open settings menu\r\n";

const code char version1[] = "\r\nAutomatic LED Lamp";
const code char version2[] = "\r\nDilshan R Jayakody";
const code char version3[] = "\r\njayakody2000lk@gmail.com";
const code char version4[] = "\r\nVersion 1.0 - 2018-11-25";

const code char inputTimeFormat[] = "HH:MM:SS > ";
const code char inputStartTime[] = "\r\nStart time ";
const code char inputStopTime[] = "\r\nStop time ";
const code char inputSystemTime[] = "\r\nSystem time ";

const code char errorNumber[] = "\r\nInvalid input!\r\n";
const code char valuePointer[] = ": ";

struct timeStruct
{
   unsigned char hour;
   unsigned char minute;
   unsigned char sec;
};

struct timeStruct _lightStartTime = {0};
struct timeStruct _lightStopTime = {0};
struct timeStruct _sysTime;

unsigned char _rtcLock = 0;
unsigned char _programIndCounter = 0;
unsigned char _programInd = 0;
unsigned char _idleCounter = 0;
unsigned char _msCounter = 0;
unsigned char _checkFlag = 0;
unsigned char _lastLightStatus = FAIL;

void initSystem();
void initSystemConfiguration();
void showError();
void setSystemTime();
void setLightStopTime();
void setLightStartTime();
void enableBlinkTimer();
void disableBlinkTimer();
void showSystemConfig();
void getSystemTime();
void loadLightSettingsFromE2PROM();
void setRTCRegister(char address, char value);
void printTimeStructure(struct timeStruct* timeData);

char* copyToRam(const char* inText);

unsigned char showMainMenu();
unsigned char getNumber();
unsigned char isLightActive();
unsigned long timeToNumber(struct timeStruct* timeData);

#endif
