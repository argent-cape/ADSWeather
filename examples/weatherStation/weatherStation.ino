/**********************************************************
** @file    weatherStation.ino
** @author    John Cape
** @copyright Argent Data Systems, Inc. - All rights reserved
**
** Example code for using the ADSWeather weather station interface library.
** This code shows how to setup and interface with the anemometer, wind vane, and rain gauge 
** found in the Argent Data Systems weather station.
** 
** The anemometer and the rain gauge each require a digital input pin that can be used as an interrupt
** The wind vane requires an analog input pin.
**/

 
#include <ADSWeather.h>

#define ANEMOMETER_PIN 2
#define VANE_PIN 0
#define RAIN_PIN 3

#define CALC_INTERVAL 1000

unsigned long nextCalc;
unsigned long timer;

int windDir;
int windSpeed;
int rainAmmount;

ADSWeather ws1(RAIN_PIN, VANE_PIN, ANEMOMETER_PIN); //This should configure all pins correctly

void setup() {
  Serial.begin(9600); 
  

  attachInterrupt(digitalPinToInterrupt(RAIN_PIN), ws1.countRain, FALLING); //ws1.countRain is the ISR for the rain gauge.
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), ws1.countAnemometer, FALLING); //ws1.countAnemometer is the ISR for the anemometer.
  nextCalc = millis() + CALC_INTERVAL;

}

void loop() {
  timer = millis();

  int rainAmmount;
  long windSpeed;
  long windDirection;
  int windGust;

  ws1.update(); //Call this every cycle in your main loop to update all the sensor values

  if(timer > nextCalc)
  {
    
    nextCalc = timer + CALC_INTERVAL;
    rainAmmount = ws1.getRain();
    windSpeed = ws1.getWindSpeed();
    windDirection = ws1.getWindDirection();
    windGust = ws1.getWindGust();

//     windSpeed / 10 will give the interger component of the wind speed
//     windSpeed % 10 will give the fractional component of the wind speed
    Serial.print("Wind speed: ");
    Serial.print(windSpeed / 10);
    Serial.print('.');
    Serial.print(windSpeed % 10);
    Serial.print(" ");

    Serial.print("Gusting at: ");
    Serial.print(windGust / 10);
    Serial.print('.');
    Serial.print(windGust % 10);
    Serial.println("");

    Serial.print("Wind Direction: ");
    Serial.print(windDirection);
    Serial.println("");

    Serial.print("Total Rain: ");
    Serial.println((float) rainAmmount / 1000);
    
  }
}

