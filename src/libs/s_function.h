#ifndef s_function_uv
#define s_function_uv

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif

#include "init_var.h"
#include "init_fun.h"

String ReturnDateTime(const DateTime& dt) {
  String date_time;
  date_time = String(dt.year())+"-"+String(dt.month())+"-"+String(dt.day())+"  "+String(dt.hour())+":"+String(dt.minute())+":"+String(dt.second());
  return date_time;
}

bool CT_Sensor(size_t Sinsor, size_t tCycle){
  bool state = true;
  if (analogRead(Sinsor) < 100)
  {
    state = true;
    delay(tCycle);
  }
  else if (analogRead(Sinsor) >= 100)
  {
    state = false;
    delay(tCycle);
  }
  return state;
}

bool SinsorState(int Sinsor){
  bool state = true;
  if (digitalRead(Sinsor) == LOW)
  {
    state = true;
  }
  else if (digitalRead(Sinsor) == HIGH)
  {
    state = false;
  }
  return state;
}

void KillSwitch(){
  vTaskDelete(RemoteHandler_t);
  xSemaphoreGive( Motion_Detected );
  vTaskSuspend(Motion_Handle);
  AlarmState = false;
  digitalWrite(Alarm,HIGH);
  detachInterrupt(digitalPinToInterrupt(MS1));
  // detachInterrupt(digitalPinToInterrupt(MS2));
  // detachInterrupt(digitalPinToInterrupt(MS3));

  digitalWrite(MainLock,HIGH);
   if (OP_Zero.group == 1)
  {
    GroupSwitch(OP_Zero.group,false);
  }
  else if (OP_Zero.group == 2)
  {
    GroupSwitch(OP_Zero.group,false);
  }
  else
  {
    GroupSwitch(OP_Zero.group,false);
  }
  vTaskResume(CT_Handle);
  OperationOnline = false;
  SaveLog(Op_ID,ReturnDateTime(OperationStartAt),"G"+String(OP_Zero.group),totalCycle,'R');
  SaveOperationID(Op_ID);
  motionStatus = "motionStop";
}

void CheckLampsLifeTime(){
  struct lampState_t GState = ReadLampState();
  if (GState.lm1>=4950)
  {
    digitalWrite(Lamp1,LOW);
  }
  if (GState.lm2>=4950)
  {
    digitalWrite(Lamp2,LOW);
  }
  if (GState.lm3>=4950)
  {
    digitalWrite(Lamp3,LOW);
  }
  if (GState.lm4>=4950)
  {
    digitalWrite(Lamp4,LOW);
  }
  if (GState.lm5>=4950)
  {
    digitalWrite(Lamp5,LOW);
  }
  if (GState.lm6>=4950)
  {
    digitalWrite(Lamp6,LOW);
  }

}


#endif
