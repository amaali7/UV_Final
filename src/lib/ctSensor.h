#ifndef ctSensors
#define ctSensors

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif
#include "motionSensor.h"


bool ctValues1[3];
bool ctValues2[3];
int CT[3] = {33, 32, 35};




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
      SaveLog(Op_ID,212,"G1",totalCycle*1000,'R');
      SaveOperationID(Op_ID);
    }
  }
  
  vTaskDelete(Motion_Handle);
}

#endif