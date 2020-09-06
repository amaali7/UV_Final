#ifndef remoteHandle11
#define remoteHandle11

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif
#include "motionSensor.h"
#include "ESPAsyncWebServer.h"

AsyncWebServer server(80);

const char* PARAM_INPUT_1 = "mode";
const char* PARAM_INPUT_2 = "interval";
const char* PARAM_INPUT_3 = "alarm";

size_t ggg;

int AlarmPin = 23;

bool OperationOnline;
int G1 = 5;
int G2 = 17;
int G3 = 16;
xTaskHandle RemoteHandler_t;

int Lamps[6] = {13,12,14,27,25,26};


struct Data_t
{
  int group;
  int time;
  bool alarm;
};


void RemoteHandle(void* parameters){
  Data_t *parm = (Data_t*) parameters;
  int group;
  totalCycle = 0;
  if (parm->group == 1)
  {
    group = G1;
  }
  else if (parm->group == 2)
  {
    group = G2;
  }
  else
  {
    group = G3;
  }
  vTaskSuspend(CT_Handle);
  vTaskResume(Motion_Handle);
  Serial.println("Current Gruop : G"+String(group)+" Time Interval : "+String(parm->time)+" Alarm State : "+String(parm->alarm) );
  if (parm->alarm)
  {
    digitalWrite(AlarmPin,HIGH);
  }
  bool firstState = true;
  Op_ID = ReadOpertionID()+1;
  
  while ( parm->time > totalCycle)  
  { 
    if (firstState)
    {
      digitalWrite(group,HIGH);
      Serial.println("First Time ^_^ ");
      firstState = false;
    }
    if( xSemaphoreTake( Motion_Detected, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
    {
      delay(1000);
      totalCycle += 1;
      xSemaphoreGive( Motion_Detected );
      Serial.println("Cycle : "+String(totalCycle));
    }
    else
    {
      digitalWrite(group,LOW);
      vTaskDelay(10); 
      Serial.println("Warnig Motion Detected ....");
    }
  }
  if (parm->alarm)
  {
    digitalWrite(AlarmPin,LOW);
    Serial.println("Operation Complete ^_^");
  }
  SaveLog(Op_ID,212,"G"+String(ggg),totalCycle,'R');
  SaveOperationID(Op_ID);
  OperationOnline = false;
  vTaskSuspend(Motion_Handle);
  vTaskResume(CT_Handle);
  vTaskDelete(RemoteHandler_t);
}




#endif