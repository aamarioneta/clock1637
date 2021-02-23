#include <TM1637Display.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "credentials.h"
#include <Timezone.h>    // https://github.com/JChristensen/Timezone
// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
Timezone CE(CEST, CET);
TimeChangeRule *tcr;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;

const int CLK = D1; //Set the CLK pin connection to the display
const int DIO = D2; //Set the DIO pin connection to the display
 
int numCounter = 0;
int tSecond = 0;
int tHour = 20;
int tMinute = 22;
int dot = 0b01000000;

 
TM1637Display display(CLK, DIO); //set up the 4-Digit Display.
 
void setup()
{
  Serial.begin(115200);
  connectWifi();
  getInternetTime();
  display.setBrightness(1); //set the diplay to maximum brightness
}
 
void loop()
{
  tSecond++;
  if (tSecond == 60) {
    tSecond = 0;
    tMinute++;
  }
  if (tMinute == 60) {
    getInternetTime();
    tMinute = 0;
    tHour++;
  }
  if (tHour == 24) {
    tHour = 0;
  }
  if (dot==0) {
    dot = 0b01000000;
  } else {
    dot = 0;
  }
  display.showNumberDecEx(tHour * 100 + tMinute, dot, true); //Display the numCounter value;
  delay(1000);
}

void connectWifi() {
  WiFi.begin(STASSID, STAPSK);
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to ");
  Serial.println(STASSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void getInternetTime() {
  timeClient.update();
  Serial.print("Got ntp time: ");
  Serial.println(timeClient.getFormattedTime());
  time_t utc = timeClient.getEpochTime();
  time_t local = CE.toLocal(utc, &tcr);
  tHour = hour(local);
  tMinute = minute(local);
  tSecond = second(local);
  Serial.print(day(local));
  Serial.print('/');
  Serial.print(month(local));
  Serial.print('/');
  Serial.print(year(local));
  Serial.print(' ');
  Serial.print(tHour);
  Serial.print(':');
  Serial.print(tMinute);
  Serial.print(':');
  Serial.println(tSecond);
}
