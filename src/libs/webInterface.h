#ifndef webInterface_uv
#define webInterface_uv

#if (ARDUINO >=100)
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif
#include "full_functions.h"




void Route(){
  

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if (OperationOnline)
    {
      request->send(SPIFFS, "/progress.html","text/html");
    }
    else
    {
      request->send(SPIFFS, "/index.html","text/html");
    }

  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/start", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String mode;
    String alarm;
    String interval;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2) && request->hasParam(PARAM_INPUT_3) && !OperationOnline) {
      mode = request->getParam(PARAM_INPUT_1)->value();
      interval = request->getParam(PARAM_INPUT_2)->value();
      alarm = request->getParam(PARAM_INPUT_3)->value();
      // todo some
      // Serial.println("Mode : "+mode+" Interval : "+interval+" Alarm : "+alarm);
      if (mode == "G1")
      {
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
        lastTime = OP_Zero.time;
        
        xTaskCreatePinnedToCore(RemoteHandle, "Main Loop", 10000, &OP_Zero,2, &RemoteHandler_t, 1);
        OperationOnline = true;
        // Serial.println("Group 1 : - Mode : "+String(OP_Zero.group)+" Interval : "+String(OP_Zero.time)+" Alarm : "+String(OP_Zero.alarm));
      }
      else if (mode == "G2")
      {
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
        lastTime = OP_Zero.time;

        xTaskCreatePinnedToCore(RemoteHandle, "Main Loop", 10000, &OP_Zero,1, &RemoteHandler_t, 1);
        OperationOnline = true;
        // Serial.println("Group 2 : - Mode : "+String(OP_Zero.group)+" Interval : "+String(OP_Zero.time)+" Alarm : "+String(OP_Zero.alarm));
      }
      else if (mode == "G3")
      {
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
        lastTime = OP_Zero.time;

        xTaskCreatePinnedToCore(RemoteHandle, "Main Loop", 10000, &OP_Zero,2, &RemoteHandler_t, 1);
        OperationOnline = true;
        // Serial.println("Group 3 : - Mode : "+String(OP_Zero.group)+" Interval : "+String(OP_Zero.time)+" Alarm : "+String(OP_Zero.alarm));
      }    
      request->send(SPIFFS, "/progress.html","text/html");
    }
    else
    {
      request->redirect("/");
    }
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/logTable", HTTP_GET, [](AsyncWebServerRequest *request){
    
      request->send(SPIFFS, "/logtable.html","text/html");
  });
    server.on("/normalize.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/normalize.css", "text/css");
  });
  
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/Javascript");
  });
  server.on("/particles.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/particles.min.js", "text/Javascript");
  });

  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/log.csv", "text/plain");
  });

  server.on("/op_state",HTTP_GET,[](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(totalCycle)+":"+String(lastTime)+":"+motionStatus);
  });
  
   server.on("/stop_now", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String KillOp;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam("KillNow") && OperationOnline) {
      KillOp = request->getParam("KillNow")->value();
      if (KillOp == "Stop")
      {
        KillSwitch();

        request->redirect("/");
      }
      else
      {
        request->send(SPIFFS, "/progress.html","text/html");

      }      
    }
    else
    {
    
      request->redirect("/");
    }
  });

  server.begin();

}

#endif