/**********************************************************
** @file		ADSWeather.cpp
** @author		John Cape
** @copyright	Argent Data Systems, Inc. - All rights reserved
**
** Argent Data Systems weather station Arduino library.
** This library provides a set of functions for interfacing
** with the Argent Data Systesm weather station sensor package
** These sensors consist of a rain gauge, a wind vane and an
** anemometer. The anemometer and the rain gauge should be 
** connected to digital input pins on one side and ground on
** the other. The weather vane is a variable resistor. 
** It should be connected to an analog input pin on one side
** and ground on the other. The analog input pin needs to be 
** connected to 5V from the Arduion through a 10K Ohm resistor.
**

*/

#ifndef ADSWeather_h
#define ADSWeather_h

#include "Arduino.h"


class ADSWeather
{
  public:
    ADSWeather(int rainPin, int windDirPin, int windSpdPin);
    
	int getRain();
	int getWindDirection();
	int getWindSpeed();
	int getWindGust();
	
	void update();
	
	static void countRain();
	static void countAnemometer();

	
  private:
	int _rainPin;
	int _windDirPin;
	int _windSpdPin;
	
	
	int _rain;
	int _windDir;
	int _windSpd;
	int _windSpdMax;
	
	unsigned long _nextCalc;
	unsigned long _timer;
  	
	unsigned int _vaneSample[50]; //50 samples from the sensor for consensus averaging
	unsigned int _vaneSampleIdx;
	unsigned int _windDirBin[16];
	
	unsigned int _gust[30]; //Array of 50 wind speed values to calculate maximum gust speed.
	unsigned int _gustIdx;
	
	int _readRainAmmount();
    int _readWindDir();
    int _readWindSpd();
	
	
	void _setBin(unsigned int windVane);
	
};

//static void countRain();

#endif