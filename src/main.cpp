#include <Arduino.h>
#include "lib/junction.h"

/****************************************************************\
        -: Pin Map :-

      Lamps :-

    l1 = 13 , l2 = 12 , l3 = 14 , l4 = 27 , l5 = 25 , l6 = 26

      Groups :-
    
    G1 = 5 , G2 = 17 , G3 16

      CT Sensors :-

    CT1 = 33 , CT2 = 32 , CT3 = 35

      Main Lock :-
    
    ML = 18

      Alarm :-
    
    AL = 23

      Motion Sensors :-
    
    MS1 = 1 , MS2 = 3 , MS3 = 19

      RTC Module :-

    SDA = 21 , SCL = 22  

\*****************************************************************/


char ssid[] = "Network-1";          //  your network SSID (name) 
char pass[] = "kdefdjrgvfdj";

void setup() {
  Serial.begin(9600);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Motion_Detected = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(CT_Loop, "CT Loop", 10000, NULL,5, &CT_Handle, 1);
  xTaskCreatePinnedToCore(Motion_Loop, "Motion Loop", 10000,NULL,2, &Motion_Handle, 1);
  vTaskSuspend(Motion_Handle);
  delay(5000);

  Setup_Wifi(ssid, pass);
  Route();
}


void loop() {
  delay(portMAX_DELAY);
}

