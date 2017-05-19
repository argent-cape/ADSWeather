/**********************************************************
** @file		ADSWeather.cpp
** @author		John Cape
** @copyright	Argent Data Systems, Inc. - All rights reserved
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