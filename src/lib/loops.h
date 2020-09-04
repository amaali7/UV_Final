#ifndef loops11
#define loops11

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif
#include "parm.h"

void Motion_Loop(void * parm){
  
  Serial.println("Mostion Sensor Opartional ");
  bool state;
  while (true)
  {  
    state = false;
    for (int i = 0; i < 3; i++)
    {
      values1[i]= SinsorState(MS[i]);
    }
    delay(motionCycile);
    for (size_t i = 0; i < 3; i++)
    {
      values2[i]= SinsorState(MS[i]);
    }
    for (size_t i = 0; i < 3; i++)
    {
      if ((values1[i] == true) || (values2[i] == true))
      {
          state = true;
          break;
      }
    }
    Serial.println("Mostion Cycle Complete ......... ");
    if (state)
    {
      digitalWrite(MainLock, LOW);
      xSemaphoreTake( Motion_Detected, ( TickType_t ) portMAX_DELAY );
      delay(5000);
    }
    else
    {
      digitalWrite(MainLock, HIGH);
      xSemaphoreGive( Motion_Detected );
    }

  }
  vTaskDelete(Motion_Handle);
}



void RemoteHandle(void* parameters){
  Data_t *parm = (Data_t*) parameters;
  int group[6];
  if (parm->group == 1)
  {
    group[0] =G1[0];
    group[1] =G1[1];
    group[2] =0;
    group[3] =0;
    group[4] =0;
    group[5] =0;
  }
  else if (parm->group == 2)
  {
    group[0] =G2[0];
    group[1] =G2[1];
    group[2] =G2[2];
    group[3] =G2[3];
    group[4] =0;
    group[5] =0;
  }
  else
  {
    group[0] =G3[0];
    group[1] =G3[1];
    group[2] =G3[2];
    group[3] =G3[3];
    group[4] =G3[4];
    group[5] =G3[5];
  }
  vTaskSuspend(CT_Handle);
  vTaskResume(Motion_Handle);
  Serial.println("Current Gruop : "+String(group[0])+"-"+String(group[1])+"-"+String(group[2])+"-"+String(group[3])+"-"+String(group[4])+"-"+String(group[5])+" Time Interval : "+String(parm->time)+" Alarm State : "+String(parm->alarm) );
  if (parm->alarm)
  {
    digitalWrite(AlarmPin,HIGH);
  }
  bool firstState = true;
  ++Op_ID;
  while ( parm->time > totalCycle)  
  { 
    if (firstState)
    {
      RelayGroup(group,true);
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
      RelayGroup(group,false);
      vTaskDelay(10); 
      Serial.println("Warnig Motion Detected ....");
    }
  }
  if (parm->alarm)
  {
    digitalWrite(AlarmPin,LOW);
    Serial.println("Operation Complete ^_^");
  }
  SaveLog(Op_ID,212,"G1",totalCycle*1000,'R');
  totalCycle = 0;
  vTaskSuspend(Motion_Handle);
  vTaskResume(CT_Handle);
  vTaskDelete(RemoteHandler_t);
}



void CT_Loop(void * parameters){
   ++Op_ID;
  while (true)  
  { 
    if( xSemaphoreTake( Motion_Detected, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
    {
      delay(1000);
      Serial.println("Current Detected  .......");
      xSemaphoreGive( Motion_Detected );
    }
    else
    {
      Serial.println("No Current Detected  .......");
      vTaskDelay(10); 
    }
  }
  SaveLog(Op_ID,212,"G1",totalCycle*1000,'R');
  vTaskDelete(Motion_Handle);
}

#endif