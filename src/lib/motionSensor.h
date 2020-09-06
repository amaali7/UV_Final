#ifndef motionSensor11
#define motionSensor11

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif
#include <SPIFFS.h>

int motionCycile = 1000;
size_t totalCycle = 0;
size_t Op_ID;

bool values1[3];
bool values2[3];
SemaphoreHandle_t Motion_Detected;
TaskHandle_t Motion_Handle;
TaskHandle_t CT_Handle;
int MainLock = 18;


int MS[3] = {1, 3, 19};

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

void SplitString(String *data, String *token, String separator)
{
  *token = data->substring(0, data->indexOf(separator));
  data->remove(0, data->indexOf(separator) + separator.length());
}

int ReadOpertionID(){
  File operatioId = SPIFFS.open("/OperationID.csv");
  
  if(!operatioId){
      Serial.println("Failed to open file for reading");
      return 0;
  }
  
  Serial.println("File Content:");
  String data = operatioId.readString();
   
  operatioId.close();
  
  return data.toInt();
}

bool SaveOperationID(size_t ID){
  bool state;

  File opertioID = SPIFFS.open("/OperationID.csv", FILE_WRITE);
  
  String id = String(ID);

  if(opertioID.println(id)){
      Serial.println("File content was appended");
      state = true;
  } else {
      Serial.println("File append failed");
      state=false;
  }

  opertioID.close();

  return state;
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
    if (!state)
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


#endif