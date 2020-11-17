#include <Arduino.h>
#include "libs/webInterface.h"

/****************************************************************\
        -: Pin Map :-

      Lamps :-

    l1 = 13 , l2 = 12 , l3 = 14 , l4 = 27 , l5 = 25 , l6 = 26

      Groups :-
    
    G1 = 23 , G2 = 4 , G3 = 2

      Motion Sensors :-
    
    MS1 = 34 , MS2 = 39 , MS3 = 36

      CT Sensors :-

    CT1 = 32 , CT2 = 33 , CT3 = 35

      Main Lock :-
    
    ML = 18

      Alarm :-
    
    AL = 5
    SAL = 5

      RTC Module :-

    SDA = 21 , SCL = 22  

\*****************************************************************/


void setup() {
  // put your setup code here, to run once:

  pinMode(Group1,OUTPUT);
  pinMode(Group2,OUTPUT);
  pinMode(Group3,OUTPUT);

  pinMode(Lamp1,OUTPUT);
  pinMode(Lamp2,OUTPUT);
  pinMode(Lamp3,OUTPUT);
  pinMode(Lamp4,OUTPUT);
  pinMode(Lamp5,OUTPUT);
  pinMode(Lamp6,OUTPUT);

  pinMode(MS1,INPUT_PULLUP);
  pinMode(MS2,INPUT_PULLUP);
  pinMode(MS3,INPUT_PULLUP);

  pinMode(MainLock,OUTPUT);
  pinMode(Alarm,OUTPUT);

  CTS1.calibrate();
  CTS2.calibrate();
  CTS3.calibrate();
  
  // Configure BUZZER functionalities.
  ledcSetup(3, 8000, 12);
  // Attach BUZZER pin.
  ledcAttachPin(SAlarm, 0);

  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(2020, 11, 7, 11, 27, 50));
  }

  OperationStartAt = rtc.now();

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  CheckLampsLifeTime();
  Motion_Detected = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(CT_Loop, "CT Loop", 10000, NULL,2, &CT_Handle, 1);
  xTaskCreatePinnedToCore(Motion_Loop, "Motion Loop", 10000,NULL,2, &Motion_Handle, 1);
  xTaskCreatePinnedToCore(Alarm_Loop, "Alarm Loop", 10000,NULL,2, &Alarm_Handle, 1);
  vTaskSuspend(Alarm_Handle);
  vTaskSuspend(Motion_Handle);

  Setup_Wifi(ssid, pass);
  Route();

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(portMAX_DELAY);
}