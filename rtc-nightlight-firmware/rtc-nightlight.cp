#line 1 "D:/projects/rtc-nightlight/rtc-nightlight.c"
#line 1 "d:/projects/rtc-nightlight/rtc-nightlight.h"






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
unsigned char _lastLightStatus =  0xFF ;

void initSystem();
void initSystemConfiguration();
unsigned char showMainMenu();
char* copyToRam(const char* inText);
unsigned char getNumber();
void showError();
void setSystemTime();
void setLightStopTime();
void setLightStartTime();
void setRTCRegister(char address, char value);
void showSystemConfig();
void printTimeStructure(struct timeStruct* timeData);
void getSystemTime();
void loadLightSettingsFromE2PROM();
void enableBlinkTimer();
void disableBlinkTimer();
unsigned char isLightActive();
unsigned long timeToNumber(struct timeStruct* timeData);
#line 3 "D:/projects/rtc-nightlight/rtc-nightlight.c"
void main()
{
 char uartData = 0;
 unsigned char lightState, lightOut;


 initSystem();
 loadLightSettingsFromE2PROM();

 _rtcLock = 0;
 _idleCounter = 0;
 _msCounter = 0;
 _checkFlag = 0;
 _programIndCounter = 0;
 _programInd = 0;


 Sound_Play(1000, 250);
 Delay_ms(100);
 Sound_Play(1250, 250);

 while(1)
 {

 if (UART1_Data_Ready())
 {
 uartData = UART1_Read();

 if((uartData == 0x20) || (uartData == 0x0D))
 {
 initSystemConfiguration();
 }
 else
 {

 UART1_Write_Text(copyToRam(intoHelp1));
 UART1_Write_Text(copyToRam(intoHelp2));
 }
 }


 if(_checkFlag == 0xFF)
 {
 _checkFlag = 0x00;
 getSystemTime();
 lightState = isLightActive();


 if(lightState != _lastLightStatus)
 {
 Sound_Play(1000, 150);
 Delay_ms(75);
 Sound_Play(1250, 150);

 _lastLightStatus = lightState;
 }


 lightOut = (lightState ==  0xFF ) ? 0x00 : 0x01;
 PORTB = (PORTB & 0xFA) | ((lightOut << 2) | ((!lightOut) & 0x01));
 }
 }
}

void Interrupt()
{

 if(TMR1IF_bit)
 {
 if((++_programIndCounter) == 2)
 {

 _programIndCounter = 0;
 _programInd = (_programInd == 0x00) ? 0x01 : 0x00;
 PORTB = (PORTB & 0xFE) | _programInd;


 if((++_idleCounter) == 60)
 {

 disableBlinkTimer();


 PCLATH = 0x00;
 PCL = 0x00;
 }
 }

 TMR1IF_bit = 0;
 TMR1H = 0x0B;
 TMR1L = 0xDC;
 }


 if(TMR0IF_bit)
 {
 TMR0IF_bit = 0x00;
 TMR0 = 0x3D;


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


 _idleCounter = 0x00;
 }
 }


 return ((inBuffer[0] - 0x30) * 10) + (inBuffer[1] - 0x30);
}

unsigned char setTimeToStruct(struct timeStruct *timeVal)
{
 unsigned char inVal;
 UART1_Write_Text(copyToRam(inputTimeFormat));


 inVal = getNumber();
 if(inVal > 23)
 {
 showError();
 return  0xFF ;
 }

 timeVal->hour = inVal;
 UART1_Write(0x3A);


 inVal = getNumber();
 if(inVal > 59)
 {
 showError();
 return  0xFF ;
 }

 timeVal->minute = inVal;
 UART1_Write(0x3A);


 inVal = getNumber();
 if(inVal > 59)
 {
 showError();
 return  0xFF ;
 }

 timeVal->sec = inVal;
 return  0x00 ;
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

 _lightStartTime.hour = EEPROM_Read(0x00);
 _lightStartTime.minute = EEPROM_Read(0x01);
 _lightStartTime.sec = EEPROM_Read(0x02);


 _lightStopTime.hour = EEPROM_Read(0x03);
 _lightStopTime.minute = EEPROM_Read(0x04);
 _lightStopTime.sec = EEPROM_Read(0x05);
}

void setLightStartTime()
{
 struct timeStruct startTime;

 UART1_Write_Text(copyToRam(inputStartTime));
 if(setTimeToStruct(&startTime) ==  0x00 )
 {

 EEPROM_Write(0x00, startTime.hour);
 EEPROM_Write(0x01, startTime.minute);
 EEPROM_Write(0x02, startTime.sec);


 _lightStartTime.hour = startTime.hour;
 _lightStartTime.minute = startTime.minute;
 _lightStartTime.sec = startTime.sec;
 }
}

void setLightStopTime()
{
 struct timeStruct stopTime;

 UART1_Write_Text(copyToRam(inputStopTime));
 if(setTimeToStruct(&stopTime) ==  0x00 )
 {

 EEPROM_Write(0x03, stopTime.hour);
 EEPROM_Write(0x04, stopTime.minute);
 EEPROM_Write(0x05, stopTime.sec);


 _lightStopTime.hour = stopTime.hour;
 _lightStopTime.minute = stopTime.minute;
 _lightStopTime.sec = stopTime.sec;
 }
}

void getSystemTime()
{

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


 I2C1_Stop();
 }
}

void setSystemTime()
{
 struct timeStruct sysTime;


 _rtcLock = 1;

 UART1_Write_Text(copyToRam(inputSystemTime));
 if(setTimeToStruct(&sysTime) ==  0x00 )
 {

 setRTCRegister(0, 0x80);
 setRTCRegister(1, Dec2Bcd(sysTime.minute));
 setRTCRegister(2, Dec2Bcd(sysTime.hour));

 setRTCRegister(4, 0x01);
 setRTCRegister(5, 0x01);
 setRTCRegister(6, 0x15);

 setRTCRegister(7, 0x00);
 setRTCRegister(0, Dec2Bcd(sysTime.sec) & 0x7F);


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

 UART1_Write_Text(copyToRam(inputStartTime));
 UART1_Write_Text(copyToRam(valuePointer));
 printTimeStructure(&_lightStartTime);


 UART1_Write_Text(copyToRam(inputStopTime));
 UART1_Write_Text(copyToRam(valuePointer));
 printTimeStructure(&_lightStopTime);


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

 enableBlinkTimer();

 while(1)
 {
 switch(showMainMenu())
 {

 case 0x01:
 setSystemTime();
 break;


 case 0x02:
 setLightStartTime();
 break;


 case 0x03:
 setLightStopTime();
 break;


 case 0x04:
 showSystemConfig();
 break;


 case 0x05:
 showSystemVersion();
 break;


 case 0x06:
 UART1_Write('\r');
 UART1_Write('\n');

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

 return  0xFF ;
 }
 else if (stopTime > startTime)
 {

 return (sysTime >= startTime) && (sysTime < stopTime) ?  0x00  :  0xFF ;
 }
 if (stopTime == 0x00)
 {

 return (sysTime < startTime) ?  0xFF  :  0x00 ;
 }
 else
 {

 return (sysTime >= startTime) || (sysTime < stopTime) ?  0x00  :  0xFF ;
 }
}

void initSystem()
{

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


 UART1_Init(9600);
 I2C1_Init(100000);
 Sound_Init(&PORTB, 1);
}
