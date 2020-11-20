#ifndef init_uv
#define init_uv

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif

#include "RTClib.h"
#include "ACS712.h"

/****************************************************************\
        -: Pin Map :-

      Lamps :-

    l1 = 13 , l2 = 12 , l3 = 14 , l4 = 27 , l5 = 25 , l6 = 26

      Groups :-
    
    G1 = 23 , G2 = 4 , G3 = 2

      CT Sensors :-

    CT1 = 32 , CT2 = 33 , CT3 = 35

      Main Lock :-
    
    ML = 18

      Alarm :-
    
    AL = 5

      Motion Sensors :-
    
    MS1 = 34 , MS2 = 39 , MS3 = 36

      RTC Module :-

    SDA = 21 , SCL = 22  

\*****************************************************************/


// Lamps pins

size_t Lamp1 = 13;
size_t Lamp2 = 12;
size_t Lamp3 = 14;
size_t Lamp4 = 27;
size_t Lamp5 = 25;
size_t Lamp6 = 26;

// Groups

size_t Group1 = 23;
size_t Group2 = 4;
size_t Group3 = 2;

// Current Sensors

size_t CT1 = 32;
size_t CT2 = 33;
size_t CT3 = 35;

ACS712 CTS1(ACS712_20A, CT1);
ACS712 CTS2(ACS712_20A, CT2);
ACS712 CTS3(ACS712_20A, CT3);

// Mottion Sensors 

size_t MS1 = 34;
size_t MS2 = 39;
size_t MS3 = 36;


// RTC Module

size_t RTC_SDA = 21;
size_t RTC_SCL = 22;

// Main Lock

size_t MainLock = 18;

// Alarm 
size_t SAlarm = 19;
size_t Alarm = 5;

// wifi Settings

    // SSID network name
char ssid[] = "Network-1";           
    // Network Password
char pass[] = "12345678"; 
    // Server port
size_t port = 80;

IPAddress IP(192, 168, 15, 1);
IPAddress gateway(192, 168, 15, 254);
IPAddress NMask(255, 255, 255, 0);

size_t motionCycile = 1000;
size_t totalCycle = 0;
size_t Op_ID;

bool values1[3];
bool values2[3];
SemaphoreHandle_t Motion_Detected;
TaskHandle_t Motion_Handle;
TaskHandle_t CT_Handle;
TaskHandle_t Alarm_Handle;
xTaskHandle RemoteHandler_t;


const char* PARAM_INPUT_1 = "mode";
const char* PARAM_INPUT_2 = "interval";
const char* PARAM_INPUT_3 = "type";

bool OperationOnline;

struct Data_t
{
  int group;
  int time;
};

struct lampState_t
{
  int lm1;
  int lm2;
  int lm3;
  int lm4;
  int lm5;
  int lm6;
};

struct CT_State_t
{
  int ct1;
  int ct2;
  int ct3;
};

int lastTime;
Data_t OP_Zero;

bool ctValues1[3];
bool ctValues2[3];

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

DateTime OperationStartAt;


#define timeSeconds 10

// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
bool controller = false;
String motionStatus;

bool AlarmState = false;
#endif