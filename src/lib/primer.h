#ifndef primer
#define primer

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif

#include <WiFi.h>
#include <SPIFFS.h>
#include "ESPAsyncWebServer.h"
#include "parm.h"
#include "loops.h"

void Setup_Wifi(char ssid[], char pass[]){

  IPAddress IP(192, 168, 15, 1);
  IPAddress gateway(192, 168, 15, 254);
  IPAddress NMask(255, 255, 255, 0);

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

void Route(){
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html","text/html");
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/start", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String mode;
    String alarm;
    String interval;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2) && request->hasParam(PARAM_INPUT_3)) {
      mode = request->getParam(PARAM_INPUT_1)->value();
      interval = request->getParam(PARAM_INPUT_2)->value();
      alarm = request->getParam(PARAM_INPUT_3)->value();
      // todo some
      Serial.println("Mode : "+mode+" Interval : "+interval+" Alarm : "+alarm);
      if (mode == "G1")
      {
        Data_t OP_Zero;
        OP_Zero.group=1;
        if (alarm == "on")
        {
          OP_Zero.alarm = true;
        }
        else if (alarm == "off")
        {
          OP_Zero.alarm = false;
        }
        OP_Zero.time = interval.toInt()*60;

        xTaskCreatePinnedToCore(RemoteHandle, "Main Loop", 10000, &OP_Zero,2, &RemoteHandler_t, 1);
        Serial.println("Group 1 : - Mode : "+String(OP_Zero.group)+" Interval : "+String(OP_Zero.time)+" Alarm : "+String(OP_Zero.alarm));
      }
      else if (mode == "G2")
      {
        Data_t OP_Zero;
        OP_Zero.group = 2;
        if (alarm == "on")
        {
          OP_Zero.alarm = true;
        }
        else if (alarm == "off")
        {
          OP_Zero.alarm = false;
        }
        OP_Zero.time = interval.toInt()*60;
        
        xTaskCreatePinnedToCore(RemoteHandle, "Main Loop", 10000, &OP_Zero,1, &RemoteHandler_t, 1);
        Serial.println("Group 2 : - Mode : "+String(OP_Zero.group)+" Interval : "+String(OP_Zero.time)+" Alarm : "+String(OP_Zero.alarm));
      }
      else if (mode == "G3")
      {
        Data_t OP_Zero;
        OP_Zero.group = 3;
        if (alarm == "on")
        {
          OP_Zero.alarm = true;
        }
        else if (alarm == "off")
        {
          OP_Zero.alarm = false;
        }
        OP_Zero.time = interval.toInt()*60;
        
        xTaskCreatePinnedToCore(RemoteHandle, "Main Loop", 10000, &OP_Zero,2, &RemoteHandler_t, 1);
        Serial.println("Group 3 : - Mode : "+String(OP_Zero.group)+" Interval : "+String(OP_Zero.time)+" Alarm : "+String(OP_Zero.alarm));
      }    
    }
    request->send(200, "text/plain", "OK");
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/Javascript");
  });
  server.on("/particles.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/particles.min.js", "text/Javascript");
  });

  
  server.begin();

}

#endif