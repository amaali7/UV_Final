#ifndef parm11
#define parm11

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif

#include "ESPAsyncWebServer.h"

AsyncWebServer server(80);

const char* PARAM_INPUT_1 = "mode";
const char* PARAM_INPUT_2 = "interval";
const char* PARAM_INPUT_3 = "alarm";

int motionCycile = 1000;

bool values1[3];
bool values2[3];

bool ctValues1[3];
bool ctValues2[3];

int MS[3] = {16, 17, 18};
int CT[3] = {16, 17, 18};
int AlarmPin = 10;
int MainLock = 15;

int G1[2] = {2,17};
int G2[4] = {2, 17, 18, 19};
int G3[6] = {2,17, 18, 19, 22, 23};

SemaphoreHandle_t Motion_Detected;
SemaphoreHandle_t CT_Operation;

TaskHandle_t Motion_Handle;
TaskHandle_t CT_Handle;


size_t totalCycle = 0;
xTaskHandle RemoteHandler_t;
size_t Op_ID = 0;

struct op_parm
{

};

struct Data_t
{
  int group;
  int time;
  bool alarm;
} ;


void SplitString(String *data, String *token, String separator)
{
  *token = data->substring(0, data->indexOf(separator));
  data->remove(0, data->indexOf(separator) + separator.length());
}

/*
  CSV file : log.csv

  "Opertion ID", "Time","Group","Operating Time", "Opertion Type"

*/

bool SaveLog(size_t op_id, long time, String group, long op_time, char op_type){
  bool state;
  //--------- Apend content to file
  File logFile = SPIFFS.open("/log.csv", FILE_APPEND);
  
  String log = String(op_id)+','+String(time)+','+group+','+String(op_time)+','+String(op_type);

  if(logFile.println(log)){
      Serial.println("File content was appended");
      state = true;
  } else {
      Serial.println("File append failed");
      state=false;
  }

  logFile.close();

  return state;
}

/*
  CSV file : lampState.csv

  "total time set 1","total time set 2","total time set 3"

*/

bool SaveState(size_t ct1, size_t ct2, size_t ct3){
  bool state;
  //--------- Apend content to file
  File logFile = SPIFFS.open("/lampState.csv", FILE_WRITE);
  
  String log = String(ct1)+','+String(ct2)+','+String(ct3);

  if(logFile.println(log)){
      Serial.println("File content was appended");
      state = true;
  } else {
      Serial.println("File append failed");
      state=false;
  }

  logFile.close();

  return state;
}

int *ReadLampState(){
  File lampstate = SPIFFS.open("/test.txt");
  int ct1;
  int ct2;
  int ct3;
  String toky;
  if(!lampstate){
      Serial.println("Failed to open file for reading");
      return 0;
  }
  
  Serial.println("File Content:");
  String data = lampstate.readString();
  SplitString(&data,&toky,",");
  ct1 = toky.toInt();
  SplitString(&data,&toky,",");
  ct2 = toky.toInt();
  SplitString(&data,&toky,",");
  ct3 = toky.toInt(); 
  lampstate.close();
  int array[3] = {ct1,ct2,ct3}; 
  return array;
}

void RelayGroup(int group[], bool state){
  if (state)
  {
    for (int i = 0; i < sizeof(group); i++)
    {
      if (group[i]!=0)
      {
        digitalWrite(group[i],HIGH);
      }     
    }
    
  }
  else
  {
    for (int i = 0; i < sizeof(group); i++)
    {
      if (group[i]!=0)
      {
        digitalWrite(group[i],LOW);
      }
    }
  }  
}

bool SinsorState(int Sinsor){
  bool state = true;
  if (digitalRead(Sinsor) == HIGH)
  {
    state = true;
  }
  else if (digitalRead(Sinsor) == LOW)
  {
    state = false;
  }
  return state;
}

bool CT_Sensor(int Sinsor){
  bool state = true;
  if (analogRead(Sinsor) < 100)
  {
    state = true;
  }
  else if (analogRead(Sinsor) >= 100)
  {
    state = false;
  }
  return state;
}


#endif