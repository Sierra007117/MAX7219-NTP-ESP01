#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <LedControl.h>
#include "WAC.h"

LedControl lc = LedControl(D4, D3, D0, 1); //DIN,CLK,CS,display

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", (6 * 60 * 60), 60000);

int SECOND_PINS[6] = {D5, D6, D7, D8, 3, 1};
int SECONDS = 0;

const char *ssid = SSID;
const char *password = PW;

void setup() {
  Serial.begin(115200);
  lc.shutdown(0,false);
  lc.setIntensity(0,15);
  lc.clearDisplay(0);

  WiFi.begin(ssid, password);
  Serial.print("Connecting.");

  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC Address: ");
  Serial.println(WiFi.macAddress());

  for(int i = 0; i < 6; i++)
    pinMode(SECOND_PINS[i], OUTPUT); 

  timeClient.begin();
}

void display() {
  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime();
  byte second_ = second(unix_epoch);
  byte last_second;
  if (last_second != second_)
  {
  //Its not stupid if it works
    byte minute_ = minute(unix_epoch);
    byte hour_ = hour(unix_epoch);
    byte day_ = day(unix_epoch);

    int Digit0 = second_ % 10;
    int Digit1 = second_ / 10;
    int Digit2 = minute_ % 10;
    int Digit3 = minute_ / 10;
    int Digit4 = hour_ % 10;
    int Digit5 = hour_ / 10;
    int Digit6 = day_ % 10;
    int Digit7 = day_ / 10;

    Serial.println(day_);
    Serial.println(hour_);
    Serial.println(minute_);
    Serial.println(second_);

    last_second = second_;

    lc.setDigit(0, 7, (byte)Digit7, false);
    lc.setDigit(0, 6, (byte)Digit6, true);
    lc.setDigit(0, 5, (byte)Digit5, false);
    lc.setDigit(0, 4, (byte)Digit4, true);
    lc.setDigit(0, 3, (byte)Digit3, false);
    lc.setDigit(0, 2, (byte)Digit2, true);
    lc.setDigit(0, 1, (byte)Digit1, false);
    lc.setDigit(0, 0, (byte)Digit0, false);
  }
}

void updateTime(int *seconds){

    
    String formattedTime;
    String stringSeconds;
    while(!timeClient.update()) {
        timeClient.forceUpdate();
    }
    // Now I know whats going on
    formattedTime = timeClient.getFormattedTime();
    stringSeconds = formattedTime.substring(6, 8); // Holy shit it worked!
    *seconds = atoi(stringSeconds.c_str());
}
void loop() { 
  display();
  updateTime(&SECONDS);  
    for(int i = 0; i < 6; i++){
        digitalWrite(SECOND_PINS[i], SECONDS % 2);
        Serial.print(SECONDS % 2);
        SECONDS /= 2;
    }  
}
