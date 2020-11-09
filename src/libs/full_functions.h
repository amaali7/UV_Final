#ifndef full_function_uv
#define full_function_uv

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif

#include "s_function.h"

void RemoteHandle(void* parameters){
  Data_t *parm = (Data_t*) parameters;
  totalCycle = 0;
  // xSemaphoreTake( Motion_Detected, ( TickType_t ) portMAX_DELAY );
  vTaskSuspend(CT_Handle);
  xSemaphoreGive( Motion_Detected );
  vTaskResume(Motion_Handle);
  attachInterrupt(digitalPinToInterrupt(MS1), detectsMovement, RISING);
  // attachInterrupt(digitalPinToInterrupt(MS2), detectsMovement, RISING);
  // attachInterrupt(digitalPinToInterrupt(MS3), detectsMovement, RISING);
  // Serial.println("Current Gruop : G"+String(parm->group)+" Time Interval : "+String(parm->time)+" Alarm State : "+String(parm->alarm) );
  if (parm->alarm)
  {
    digitalWrite(Alarm,HIGH);
  }
  bool firstState = true;
  Op_ID = ReadOpertionID()+1;
  
  while ( parm->time > totalCycle)  
  { 
    if (firstState)
    {
      // digitalWrite(MainLock,LOW);
      GroupSwitch(parm->group,true);
      
      firstState = false;
      OperationStartAt = rtc.now();
      Serial.println("First Time ^_^ ");
    }
    if( xSemaphoreTake( Motion_Detected, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
    {
      delay(1000);
      totalCycle += 1;
      xSemaphoreGive( Motion_Detected );
      Serial.println("Cycle : "+String(totalCycle));
      // if (bitRead(MainLock,3)==1)
      // {
      //   digitalWrite(MainLock,LOW);
      // }
    }
    else
    {
      // if (bitRead(MainLock,3)==0)
      // {
      //   digitalWrite(MainLock,HIGH);
      // }

       
      Serial.println("Warnig Motion Detected ....");
    }
    vTaskDelay(10);
  }
  if (parm->alarm)
  {
    digitalWrite(Alarm,LOW);
  }
  GroupSwitch(parm->group,false);
  digitalWrite(MainLock,LOW);
  // Serial.println("Operation Complete ^_^");
  SaveLog(Op_ID,ReturnDateTime(OperationStartAt),"G"+String(parm->group+1),totalCycle,'R');
  SaveOperationID(Op_ID);
  OperationOnline = false;
  detachInterrupt(digitalPinToInterrupt(MS1));
  // detachInterrupt(digitalPinToInterrupt(MS2));
  // detachInterrupt(digitalPinToInterrupt(MS3));
  xSemaphoreTake( Motion_Detected, ( TickType_t ) portMAX_DELAY );
  vTaskSuspend(Motion_Handle);
  // vTaskResume(CT_Handle);
  xSemaphoreGive( Motion_Detected );
  vTaskDelete(RemoteHandler_t);
  
}



void Motion_Loop(void * parm){
  
  Serial.println("Mostion Sensor Opartional ");
  while (true)
  {  
    if (controller)
      {
        xSemaphoreTake( Motion_Detected, ( TickType_t ) portMAX_DELAY );
        digitalWrite(MainLock, HIGH);
        Serial.println("MOTION DETECTED interrpts Start");
        controller = false;
      }
    now = millis();
    if(startTimer && (now - lastTrigger > (timeSeconds*1000))) {
      Serial.println("Motion stopped interrpts End");
      digitalWrite(MainLock, LOW);
      startTimer = false;
      xSemaphoreGive( Motion_Detected );
    }
    vTaskDelay(10);
  }
  vTaskDelete(Motion_Handle);
}


void CT_Loop(void * parameters){
   
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
      Op_ID = ReadOpertionID()+1;
      SaveLog(Op_ID,ReturnDateTime(OperationStartAt),"G1",totalCycle*1000,'R');
      SaveOperationID(Op_ID);
    }
    vTaskDelay(10);
  }
  vTaskDelete(Motion_Handle);
}

#endif