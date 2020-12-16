#ifndef init_fun_uv
#define init_fun_uv

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif

#include <WiFi.h>
#include <SPIFFS.h>
#include "ESPAsyncWebServer.h"
#include "init_var.h"


AsyncWebServer server(80);
RTC_DS3231 rtc;  //  rtc class

void Setup_Wifi(char ssid[], char pass[]){

  Serial.println();
  Serial.print("System Start");

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid,pass,1,0,1);
  delay(1000);

  WiFi.softAPConfig(IP, gateway, NMask);

  delay(1000);

  IPAddress myIP = WiFi.softAPIP();
  Serial.println();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

}

void SplitString(String *data, String *token, String separator)
{
  *token = data->substring(0, data->indexOf(separator));
  data->remove(0, data->indexOf(separator) + separator.length());
}

int ReadOpertionID(){
  File operatioId = SPIFFS.open("/OperationID.csv");

  if(!operatioId){
      // Serial.println("Failed to open file for reading");
      return 0;
  }

  // Serial.println("File Content:");
  String data = operatioId.readString();

  operatioId.close();

  return data.toInt();
}

bool SaveOperationID(size_t ID){
  bool state;

  File opertioID = SPIFFS.open("/OperationID.csv", FILE_WRITE);

  String id = String(ID);

  if(opertioID.println(id)){
      // Serial.println("File content was appended");
      state = true;
  } else {
      // Serial.println("File append failed");
      state=false;
  }

  opertioID.close();

  return state;
}



/*
  CSV file : log.csv

  "Opertion ID", "Time","Group","Operating Time", "Opertion Type"

*/

bool SaveLog(size_t op_id, String time, String group, long op_time, char op_type){
  bool state;
  //--------- Apend content to file
  File logFile = SPIFFS.open("/log.csv", FILE_APPEND);

  String log = String(op_id)+','+time+','+group+','+String(op_time)+','+String(op_type);

  if(logFile.println(log)){
      // Serial.println("File content was appended");
      state = true;
  } else {
      // Serial.println("File append failed");
      state=false;
  }

  logFile.close();

  return state;
}

/*
  CSV file : lampState.csv

  "total time set 1","total time set 2","total time set 3"

*/

bool SaveState(size_t G1=0, size_t G2=0, size_t G3=0){
  bool state;
  //--------- Apend content to file
  File logFile = SPIFFS.open("/lampState.csv", FILE_WRITE);
  String data = logFile.readString();
  String token;
  SplitString(&data,&token,",");
  int lmt1 = token.toInt();
  SplitString(&data,&token,",");
  int lmt2 = token.toInt();
  SplitString(&data,&token,",");
  int lmt3 = token.toInt();
  SplitString(&data,&token,",");
  int lmt4 = token.toInt();
  SplitString(&data,&token,",");
  int lmt5 = token.toInt();
  int lmt6 = data.toInt();

  if (G1 != 0)
  {
    lmt1+=G1;
    lmt2+=G1;
  }
  if (G2 != 0)
  {
    lmt3+=G2;
    lmt4+=G2;
  }
  if (G3 != 0)
  {
    lmt5+=G3;
    lmt6+=G3;
  }
  if(logFile.println(String(lmt1)+","+String(lmt2)+","+String(lmt3)+","+String(lmt4)+","+String(lmt5)+","+String(lmt6))){
      Serial.println("Lamp State Updated ...");
      state = true;
  } else {
      Serial.println("Lamp State Update failed");
      state=false;
  }

  logFile.close();

  return state;
}

struct lampState_t ReadLampState(){
  File operatioId = SPIFFS.open("/lampState.csv");

  if(!operatioId){
      Serial.println("Failed to open LampState.csv for reading");
  }

  // Serial.println("File Content:");
  String data = operatioId.readString();
  String token;
  SplitString(&data,&token,",");
  int lmt1 = token.toInt();
  SplitString(&data,&token,",");
  int lmt2 = token.toInt();
  SplitString(&data,&token,",");
  int lmt3 = token.toInt();
  SplitString(&data,&token,",");
  int lmt4 = token.toInt();
  SplitString(&data,&token,",");
  int lmt5 = token.toInt();
  int lmt6 = data.toInt();

  return {lmt1,lmt2,lmt3,lmt4,lmt5,lmt6};
}

void GroupSwitch(size_t group, bool state){
  if (state)
  {
    if (group == 1)
    {
      digitalWrite(Group1, LOW);
      // Serial.println("G1 Operating ...");
    }
    else if (group == 2)
    {
      digitalWrite(Group1, LOW);
      digitalWrite(Group2, LOW);
      // Serial.println("G2 Operating ...");
    }
    else if (group == 3)
    {
      digitalWrite(Group1, LOW);
      digitalWrite(Group2, LOW);
      digitalWrite(Group3, LOW);
      // Serial.println("G3 Operating ...");
    }
  }
  if (!state)
  {
    if (group == 1)
    {
      digitalWrite(Group1, HIGH);
      // Serial.println("G1 is Down ...");
    }
    else if (group == 2)
    {
      digitalWrite(Group1, HIGH);
      digitalWrite(Group2, HIGH);
      // Serial.println("G2 is Down ...");
    }
    else if (group == 3)
    {
      digitalWrite(Group1, HIGH);
      digitalWrite(Group2, HIGH);
      digitalWrite(Group3, HIGH);
      // Serial.println("G3 is Down ...");
    }
  }
}

// Checks if motion was detected, sets LED LOW and starts a timer
void IRAM_ATTR detectsMovement() {
  if(!startTimer ) {
    startTimer = true;
    controller = true;
    motionStatus = "motionDetected";
    lastTrigger = millis();
  }
  // else
  // {
  //   Serial.println("TimeTrigger Updated .");
  //   lastTrigger = millis();
  //   AlarmState = true;
  // }
}

// struct CT_State_t CTS_State(){
//   struct CT_State_t tempo;
//   tempo.ct1 = int(CTS1.getCurrentAC(60));
//   tempo.ct2 = int(CTS2.getCurrentAC(60));
//   tempo.ct3 = int(CTS3.getCurrentAC(60));
//   return tempo;
// }

String GroupFinder(int Groups[3]){
  String data = "";
  for (int i = 0; i < 3; i++)
  {
    if (Groups[i]==1)
    {
      data = data + "-" + String(i+1);
    }
  }
  return data;
}

#endif
