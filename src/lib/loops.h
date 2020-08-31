#ifndef loops11
#define loops11

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif
#include "parm.h"

void Motion_Loop(void * parm){
  int *ms = (int*) parm;

  bool state;
  while (true)
  {
    if (ms == 0)
    {
      state = false;
      for (int i = 0; i < 3; i++)
      {
      values1[i]= SinsorState(ms[1]);
      }
      delay(motionCycile);
      for (size_t i = 0; i < 3; i++)
      {
      values2[i]= SinsorState(ms[i]);
      }
      for (size_t i = 0; i < 3; i++)
      {
      if ((values1[i] == true) || (values2[i] == true))
      {
          state = true;
          break;
      }
      }
      if (state)
      {
      xSemaphoreTake( RemoteHandler_t, ( TickType_t ) portMAX_DELAY );
      delay(5000);
      }
      else
      {
          xSemaphoreGive( RemoteHandler_t );
      }
    }
    else
    {
      state = false;
      for (int i = 0; i < 3; i++)
      {
      values1[i]= SinsorState(ms[1]);
      }
      delay(motionCycile);
      for (size_t i = 0; i < 3; i++)
      {
      values2[i]= SinsorState(ms[i]);
      }
      for (size_t i = 0; i < 3; i++)
      {
      if ((values1[i] == true) || (values2[i] == true))
      {
          state = true;
          break;
      }
      }
      if (state)
      {
        digitalWrite(MainLock, LOW);
      }
      else
      {
        digitalWrite(MainLock, HIGH);
      }
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
  }
  else if (parm->group == 2)
  {
    group[0] =G2[0];
    group[1] =G2[1];
    group[2] =G2[2];
    group[3] =G2[3];
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
  xTaskCreatePinnedToCore(Motion_Loop, "Main Loop", 10000,(void*) 1,2, &Motion_Handle, 1);
  if (parm->alarm)
  {
    digitalWrite(AlarmPin,HIGH);
  }
  bool firstState = false;
  ++Op_ID;
  while (totalCycle < parm->time)  
  { 
    if (!firstState)
    {
      RelayGroup(group,true);
    }
    if( xSemaphoreTake( Motion_Detected, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
    {
      delay(1000);
      totalCycle += 1;
      xSemaphoreGive( Motion_Detected );
    }
    else
    {
      RelayGroup(group,false);
      vTaskDelay(10); 
    }
  }
  if (parm->alarm)
  {
    digitalWrite(AlarmPin,LOW);
  }
  SaveLog(Op_ID,212,"G1",totalCycle*1000,'R');
  vTaskResume(CT_Handle);
  vTaskDelete(Motion_Handle);
  vTaskDelete(RemoteHandler_t);
}



void CT_Loop(void * parameters){
  Data_t *parm = (Data_t*) parameters;
  xTaskCreatePinnedToCore(Motion_Loop, "Main Loop", 10000,(void*) 0,2, &Motion_Handle, 1);
   ++Op_ID;
  while (true)  
  { 
    if( xSemaphoreTake( Motion_Detected, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
    {
      delay(1000);
      totalCycle += 1;
      xSemaphoreGive( Motion_Detected );
    }
    else
    {
      
      vTaskDelay(10); 
    }
  }
  SaveLog(Op_ID,212,"G1",totalCycle*1000,'R');
  vTaskDelete(Motion_Handle);
}

#endif