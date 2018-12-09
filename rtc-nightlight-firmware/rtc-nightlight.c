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

#include "rtc-nightlight.h"

void main()
{
   char uartData = 0;
   unsigned char lightState, lightOut;

   // Initialize system registers and peripherals.
   initSystem();
   loadLightSettingsFromE2PROM();
   
   _rtcLock = 0;
   _idleCounter = 0;
   _msCounter = 0;
   _checkFlag = 0;
   _programIndCounter = 0;
   _programInd = 0;
   
   // Generate initial startup beeps.
   Sound_Play(1000, 250);
   Delay_ms(100);
   Sound_Play(1250, 250);
   
   while(1)
   {
      // Check for any data in serial buffer to enter into configuration options.
      if (UART1_Data_Ready())
      {
         uartData = UART1_Read();
         // Configuration option is allow for <ENTER> or <SPACE> key strokes.
         if((uartData == 0x20) || (uartData == 0x0D))
         {
            initSystemConfiguration();
         }
         else
         {
           // For non <ENTER> or <SPACE> key strokes show time and quick help.
           UART1_Write_Text(copyToRam(intoHelp1));
           UART1_Write_Text(copyToRam(intoHelp2));
         }
      }
      
      // Check for system time update flag.
      if(_checkFlag == 0xFF)
      {
         _checkFlag = 0x00;
         getSystemTime();
         lightState = isLightActive();
         
         // Beep if state change occured.
         if(lightState != _lastLightStatus)
         {
            Sound_Play(1000, 150);
            Delay_ms(75);
            Sound_Play(1250, 150);
            
            _lastLightStatus = lightState;
         }
         
         // Update state of the main lamp and status LED.
         lightOut = (lightState == FAIL) ? 0x00 : 0x01;
         PORTB = (PORTB & 0xFA) |  ((lightOut << 2) | ((!lightOut) & 0x01));
      }
   }
}

void Interrupt()
{
   // Check for TIMER01 interrupts.
   if(TMR1IF_bit)
   {
      if((++_programIndCounter) == 2)
      {
         // Flash status LED to indicate programming mode.
         _programIndCounter = 0;
         _programInd = (_programInd == 0x00) ? 0x01 : 0x00;
         PORTB = (PORTB & 0xFE) | _programInd;
         
         // Increase idle counter.
         if((++_idleCounter) == 60)
         {
            // Getting ready to restart the system.
            disableBlinkTimer();
            
            // Reset system to leave the programming mode.
            PCLATH = 0x00;
            PCL = 0x00;
         }
      }
      // Start TIMER01 again.
      TMR1IF_bit = 0;
      TMR1H = 0x0B;
      TMR1L = 0xDC;
   }
   
   // Check for TIMER0 Interrupt.
   if(TMR0IF_bit)
   {
      TMR0IF_bit = 0x00;
      TMR0 = 0x3D;
      
      // Raise flag to update the system time.
      if((++_msCounter) == 20)
      {
         _msCounter = 0;
         _checkFlag = 0xFF;
      }
   }
}

void enableBlinkTimer()
{
   _programIndCounter = 0;
   _programInd = 0;
   
   // Set TIMER01 to get trigger in 500ms intervals.
   T1CON = 0x31;
   TMR1IF_bit = 0;
   TMR1H = 0x0B;
   TMR1L = 0xDC;
   TMR1IE_bit = 1;
}

void disableBlinkTimer()
{
   _programIndCounter = 0;
   _programInd = 0;
   
   T1CON = 0x00;
}

void setRTCRegister(char address, char value)
{
   I2C1_Start();
   I2C1_Wr(0xD0);
   I2C1_Wr(address);
   I2C1_Wr(value);
   I2C1_Stop();
}

char* copyToRam(const char* inText)
{
   static char returnBuffer[32];
   unsigned char pos = 0;
   for(pos = 0; returnBuffer[pos] = inText[pos]; pos++);
   return returnBuffer;
}

void showError()
{
   Sound_Play(1500, 250);
   UART1_Write_Text(copyToRam(errorNumber));
}

unsigned char getNumber()
{
   unsigned char inBuffer[2];
   unsigned char pos = 0;
   
   // Capture numbers from the user input.
   while(1)
   {
      if(UART1_Data_Ready())
      {
         inBuffer[pos] = UART1_Read();
         if((inBuffer[pos] > 0x2F) && (inBuffer[pos] < 0x3A))
         {
            UART1_Write(inBuffer[pos]);
            if((++pos) == 2)
            {
               break;
            }
         }
         
         // Reset idle counter.
         _idleCounter = 0x00;
      }
   }
   
   // Convert string to a number.
   return ((inBuffer[0] - 0x30) * 10) + (inBuffer[1] - 0x30);
}

unsigned char setTimeToStruct(struct timeStruct *timeVal)
{
   unsigned char inVal;
   UART1_Write_Text(copyToRam(inputTimeFormat));
   
   // Process hours.
   inVal = getNumber();
   if(inVal > 23)
   {
       showError();
       return FAIL;
   }
   
   timeVal->hour = inVal;
   UART1_Write(0x3A);
   
   // Process minutes.
   inVal = getNumber();
   if(inVal > 59)
   {
       showError();
       return FAIL;
   }
   
   timeVal->minute = inVal;
   UART1_Write(0x3A);
   
   // Process seconds.
   inVal = getNumber();
   if(inVal > 59)
   {
       showError();
       return FAIL;
   }
   
   timeVal->sec = inVal;
   return SUCCESS;
}

unsigned char showMainMenu()
{
   char userSelection = 0;
   
   UART1_Write_Text(copyToRam(helpText1));
   UART1_Write_Text(copyToRam(helpText1A));
   UART1_Write_Text(copyToRam(helpText2));
   UART1_Write_Text(copyToRam(helpText3));
   UART1_Write_Text(copyToRam(helpText4));
   UART1_Write_Text(copyToRam(helpText5));
   UART1_Write_Text(copyToRam(helpText6));
   UART1_Write_Text(copyToRam(helpText7));
   UART1_Write_Text(copyToRam(helpText8));

   while(1)
   {
      if(UART1_Data_Ready())
      {
         userSelection = UART1_Read();

         // Wait for valid user input.
         if((userSelection > 0x30) && (userSelection < 0x37))
         {
            UART1_Write(userSelection);
            _idleCounter = 0x00;
            return userSelection - 0x30;
         }
      }
   }
}

void loadLightSettingsFromE2PROM()
{
   // Get light start time from the E2PROM.
   _lightStartTime.hour = EEPROM_Read(0x00);
   _lightStartTime.minute = EEPROM_Read(0x01);
   _lightStartTime.sec = EEPROM_Read(0x02);
   
   // Get light stop time from the E2PROM.
   _lightStopTime.hour = EEPROM_Read(0x03);
   _lightStopTime.minute = EEPROM_Read(0x04);
   _lightStopTime.sec = EEPROM_Read(0x05);
}

void setLightStartTime()
{
   struct timeStruct startTime;
   
   UART1_Write_Text(copyToRam(inputStartTime));
   if(setTimeToStruct(&startTime) == SUCCESS)
   {
      // Write light start time into E2PROM address 0x00-0x02.
      EEPROM_Write(0x00, startTime.hour);
      EEPROM_Write(0x01, startTime.minute);
      EEPROM_Write(0x02, startTime.sec);
      
      // Update system data structure with new configuration.
      _lightStartTime.hour = startTime.hour;
      _lightStartTime.minute = startTime.minute;
      _lightStartTime.sec = startTime.sec;
   }
}

void setLightStopTime()
{
   struct timeStruct stopTime;

   UART1_Write_Text(copyToRam(inputStopTime));
   if(setTimeToStruct(&stopTime) == SUCCESS)
   {
      // Write light stop time into E2PROM address 0x03-0x05.
      EEPROM_Write(0x03, stopTime.hour);
      EEPROM_Write(0x04, stopTime.minute);
      EEPROM_Write(0x05, stopTime.sec);
      
      // Update system data structure with new configuration.
      _lightStopTime.hour = stopTime.hour;
      _lightStopTime.minute = stopTime.minute;
      _lightStopTime.sec = stopTime.sec;
   }
}

void getSystemTime()
{
   // Get RTC time if RTC lock is released, otherwise ignore this call.
   if(_rtcLock == 0)
   {
     I2C1_Start();
     I2C1_Wr(0xD0);
     I2C1_Wr(0);
     I2C1_Start();
     I2C1_Wr(0xD1);

     _sysTime.sec = Bcd2Dec(I2C1_Rd(1));
     _sysTime.minute = Bcd2Dec(I2C1_Rd(1));
     _sysTime.hour = Bcd2Dec(I2C1_Rd(0));

     // Skip day, date, month and year values.
     I2C1_Stop();
   }
}

void setSystemTime()
{
   struct timeStruct sysTime;

   // Set RTC lock to avoid any overlap RTC operations.
   _rtcLock = 1;
   
   UART1_Write_Text(copyToRam(inputSystemTime));
   if(setTimeToStruct(&sysTime) == SUCCESS)
   {
      // Stop RTC oscillator and set new time configuration.
      setRTCRegister(0, 0x80);
      setRTCRegister(1, Dec2Bcd(sysTime.minute));
      setRTCRegister(2, Dec2Bcd(sysTime.hour));
      // We don't care about date, month and year. Lets configure some fixed values to those parameters.
      setRTCRegister(4, 0x01);
      setRTCRegister(5, 0x01);
      setRTCRegister(6, 0x15);
      // Disable SQW output and start oscillator.
      setRTCRegister(7, 0x00);
      setRTCRegister(0, Dec2Bcd(sysTime.sec) & 0x7F);
      
      // Update global data structure.
      _sysTime.sec = sysTime.sec;
      _sysTime.minute = sysTime.minute;
      _sysTime.hour = sysTime.hour;
   }
   
   _rtcLock = 0;
}

void printDigit(unsigned char val)
{
   unsigned char tempDigit = val / 10;
   UART1_Write(tempDigit + 0x30);
   tempDigit = val - (tempDigit * 10);
   UART1_Write(tempDigit + 0x30);
}

void printTimeStructure(struct timeStruct* timeData)
{
   printDigit(timeData->hour);
   UART1_Write(':');
   printDigit(timeData->minute);
   UART1_Write(':');
   printDigit(timeData->sec);
}

void showSystemConfig()
{
   // Show light start time.
   UART1_Write_Text(copyToRam(inputStartTime));
   UART1_Write_Text(copyToRam(valuePointer));
   printTimeStructure(&_lightStartTime);
   
   // Show light stop time.
   UART1_Write_Text(copyToRam(inputStopTime));
   UART1_Write_Text(copyToRam(valuePointer));
   printTimeStructure(&_lightStopTime);
   
   // Show current system time.
   UART1_Write_Text(copyToRam(inputSystemTime));
   UART1_Write_Text(copyToRam(valuePointer));
   getSystemTime();
   printTimeStructure(&_sysTime);
}

void showSystemVersion()
{
   UART1_Write_Text(copyToRam(version1));
   UART1_Write_Text(copyToRam(version2));
   UART1_Write_Text(copyToRam(version3));
   UART1_Write_Text(copyToRam(version4));
}

void initSystemConfiguration()
{
   // Start TIMER01 to blink the status LED.
   enableBlinkTimer();
   
   while(1)
   {
      switch(showMainMenu())
      {
         // Set system time.
         case 0x01:
            setSystemTime();
            break;
            
         // Set start time.
         case 0x02:
            setLightStartTime();
            break;

         // Set end time.
         case 0x03:
            setLightStopTime();
            break;

         // Show system configuration.
         case 0x04:
            showSystemConfig();
            break;
         
         // Show version information of the system.
         case 0x05:
            showSystemVersion();
            break;
            
         // Exit from the configuration system.
         case 0x06:
            UART1_Write('\r');
            UART1_Write('\n');
            // Shutdown TIMER01 and reset status LED.
            disableBlinkTimer();
            PORTB = PORTB & 0xFE;
            return;
            break;
      }
      
      UART1_Write('\r');
      UART1_Write('\n');
   }
}

unsigned long timeToNumber(struct timeStruct* timeData)
{
   unsigned long hour = timeData->hour;
   unsigned long mins = timeData->minute;
   unsigned long sec = timeData->sec;
   
   return (hour << 16) | (mins << 8) | (sec);
}

unsigned char isLightActive()
{
   unsigned long startTime = timeToNumber(&_lightStartTime);
   unsigned long stopTime = timeToNumber(&_lightStopTime);
   unsigned long sysTime= timeToNumber(&_sysTime);

   if (stopTime == startTime)
   {
      // Nothing happens if start and end times are equal.
      return FAIL;
   }
   else if (stopTime > startTime)
   {
      // Start and end time are defined before midnight.
      return (sysTime >= startTime) && (sysTime < stopTime) ? SUCCESS : FAIL;
   }
   if (stopTime == 0x00)
   {
      // Stop time is set to mid night.
      return (sysTime < startTime) ? FAIL : SUCCESS;
   }
   else
   {
       // Start and end time are defined including midnight.
       return (sysTime >= startTime) || (sysTime < stopTime) ? SUCCESS : FAIL;
   }
}

void initSystem()
{
   // Setup system registers to default state.
   TRISB = 0x00;
   PORTB = 0x00;
   ADCON0 = 0x00;
   ANSEL = 0x00;
   ANSELH = 0x00;
   WPUB = 0x00;
   CCP1CON = 0x00;
   CM1CON0 = 0x00;
   CM2CON0 = 0x00;
   INTCON = 0xE0;
   TMR0 = 0x3D;
   OPTION_REG = 0x07;
   
   // Enable I2C and UART communication channels.
   UART1_Init(9600);
   I2C1_Init(100000);
   Sound_Init(&PORTB, 1);
}
