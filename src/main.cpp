#include <Arduino.h>
#include "lib/primer.h"

char ssid[] = "Network-1";          //  your network SSID (name) 
char pass[] = "kdefdjrgvfdj";

void setup() {
  Serial.begin(9600);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  CT_Operation = xSemaphoreCreateMutex();
  Motion_Detected = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(CT_Loop, "CT Loop", 10000, NULL,1, &CT_Handle, 1);
  xTaskCreatePinnedToCore(Motion_Loop, "Motion Loop", 10000,NULL,2, &Motion_Handle, 1);
  vTaskSuspend(Motion_Handle);
  delay(5000);

  Setup_Wifi(ssid, pass);
  Route();
}


void loop() {
  delay(portMAX_DELAY);
}

