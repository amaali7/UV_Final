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
  OperationOnline = true;
  // vTaskSuspend(CT_Handle);
  vTaskResume(Motion_Handle);
  attachInterrupt(digitalPinToInterrupt(MS1), detectsMovement, RISING);
  // attachInterrupt(digitalPinToInterrupt(MS2), detectsMovement, RISING);
  // attachInterrupt(digitalPinToInterrupt(MS3), detectsMovement, RISING);
  AlarmState = true;
  digitalWrite(Alarm,LOW);
  delay(10000);
  AlarmState = false;
  bool firstState = true;
  Op_ID = ReadOpertionID()+1;

  while ( parm->time > totalCycle)
  {
    if (firstState)
    {
      digitalWrite(MainLock,HIGH);
      xSemaphoreGive( Motion_Detected );
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
    }
    vTaskDelay(50);
  }
  digitalWrite(Alarm,HIGH);
  GroupSwitch(parm->group,false);
  digitalWrite(MainLock,HIGH);
  motionStatus = "motionStop";
  SaveLog(Op_ID,ReturnDateTime(OperationStartAt),"G"+String(parm->group+1),totalCycle,'R');
  SaveOperationID(Op_ID);
  OperationOnline = false;
  detachInterrupt(digitalPinToInterrupt(MS1));
  // detachInterrupt(digitalPinToInterrupt(MS2));
  // detachInterrupt(digitalPinToInterrupt(MS3));
  xSemaphoreGive( Motion_Detected );
  vTaskSuspend(Motion_Handle);
  // vTaskResume(CT_Handle);
  vTaskDelete(RemoteHandler_t);
}



void Motion_Loop(void * parm){

  Serial.println("Mostion Sensor Opartional ");
  while (true)
  {
    if (controller)
      {
        xSemaphoreTake( Motion_Detected, ( TickType_t ) portMAX_DELAY );
        digitalWrite(MainLock, LOW);
        AlarmState = true;
        Serial.println("MOTION DETECTED interrpts Start");
        controller = false;
      }
    now = millis();
    if(startTimer && (now - lastTrigger > (timeSeconds*1000))) {
      Serial.println("Motion stopped interrpts End");
      digitalWrite(MainLock, HIGH);
      startTimer = false;
      motionStatus = "motionStop";
      AlarmState = false;
      xSemaphoreGive( Motion_Detected );
    }
    vTaskDelay(50);
  }
  vTaskDelete(Motion_Handle);
}


// void CT_Loop(void * parameters){
//   int group[3] ;
//   /*
//     Operation State refare to start end wating

//     start      = 0
//     opertating = 1
//     end        = 2
//     wating     = 3
//   */
//   size_t watingTime = 0;
//   size_t OperationState = 2;
//   struct CT_State_t ct_cache;
//   while (true)
//   {
//     totalCycle = 0;
//     // if current detected start operation
//     ct_cache = CTS_State();
//     if (ct_cache.ct1 == 1 || ct_cache.ct2 == 1 || ct_cache.ct3 == 1 )
//     {
//       if (ct_cache.ct1 > 0)
//       {
//         group[0]=1;
//         OperationState = 0;
//       }
//       else
//       {
//         group[0]=0;
//       }

//       if (ct_cache.ct2 > 0)
//       {
//         group[1]=1;
//         OperationState = 0;
//       }
//       else
//       {
//         group[1]=0;
//       }
//       if (ct_cache.ct3 > 0)
//       {
//         group[2]=1;
//         OperationState = 0;
//       }
//       else
//       {
//         group[2]=0;
//       }
//     }

//     if (OperationState == 0)
//     {
//       // group that detect current
//       digitalWrite(Alarm,LOW);
//       OperationState = 1;
//       Op_ID = ReadOpertionID()+1;
//       OperationStartAt = rtc.now();
//     }

//     while (OperationState == 1)
//     {
//       // delay(1000);

//       ct_cache = CTS_State();
//       if (ct_cache.ct1 == 1 || ct_cache.ct2 == 1 || ct_cache.ct3 == 1 )
//       {
//         if( xSemaphoreTake( Motion_Detected, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
//         {
//           delay(1000);
//           ++totalCycle;
//           xSemaphoreGive( Motion_Detected );
//           watingTime = 0;
//         }
//       }
//       else
//       {
//         // OperationState = 1;
//         delay(1000);
//         ++watingTime;
//       }
//       if (watingTime > 5)
//       {
//         break;
//         OperationState  = 2;
//       }

//     }
//     if(OperationState == 2)
//     {
//       SaveLog(Op_ID,ReturnDateTime(OperationStartAt),"G"+GroupFinder(group),totalCycle,'L');
//       digitalWrite(Alarm,HIGH);
//       SaveOperationID(Op_ID);
//       OperationState = 3;
//       totalCycle = 0;
//     }
//     vTaskDelay(50);
//   }
//   vTaskDelete(Motion_Handle);
// }

void Alarm_Loop(void * parm){

  while (true)
  {
    while (AlarmState)
    {
      delay(100);
      ledcWriteTone(0, 5000);
      delay(100);
      ledcWriteTone(0, 0);
    }
    vTaskDelay(50);
  }
  vTaskDelete(Alarm_Handle);
}
#endif
