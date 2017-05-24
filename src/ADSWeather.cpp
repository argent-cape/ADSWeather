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

#include "Arduino.h"
#include "ADSWeather.h"

#define DEBOUNCE_TIME 15
#define CALC_INTERVAL 1000

volatile int _anemometerCounter;
volatile int _rainCounter;
volatile unsigned long last_micros_rg;
volatile unsigned long last_micros_an;


//Initialization routine. This functrion sets up the pins on the Arduino and initializes variables.
ADSWeather::ADSWeather(int rainPin, int windDirPin, int windSpdPin)
{
	
	
  //Initialization routine
  _anemometerCounter = 0;
  _rainCounter = 0;
  _gustIdx = 0;
  _vaneSampleIdx = 0;
  
  _rainPin = rainPin;
  _windDirPin = windDirPin;
  _windSpdPin = windSpdPin;
  

  pinMode(_rainPin, INPUT);
  digitalWrite(_rainPin, HIGH);
  pinMode(_windSpdPin, INPUT);
  digitalWrite(_windSpdPin, HIGH);

  pinMode(_windDirPin, INPUT);
  }

//The update function updates the values of all of the sensor variables. This should be run as frequently as possible
//in the main loop for maximum precision.
void ADSWeather::update()
{
	_timer = millis();
	_vaneSample[_vaneSampleIdx] = analogRead(_windDirPin);
	_vaneSampleIdx++;
	if(_vaneSampleIdx >= 50)
	{
		_vaneSampleIdx=0;
	}
	if(_timer > _nextCalc)
	{
		_nextCalc = _timer + CALC_INTERVAL;
 
		//UPDATE ALL VALUES
		_rain += _readRainAmmount();
		_windSpd = _readWindSpd();
		
		_windDir = _readWindDir();

	}
}

//Returns the ammount of rain since the last time the getRain function was called.
int ADSWeather::getRain()
{
	return _rain;
}

//Returns the direction of the wind in degrees.
int ADSWeather::getWindDirection()
{
	return _windDir;
}

//Returns the wind speed.
int ADSWeather::getWindSpeed()
{
	return _windSpd;
}

//Returns the maximum wind gust speed. 
int ADSWeather::getWindGust()
{
	return _windSpdMax;
}
 
//Updates the rain ammmount internal state.
int ADSWeather::_readRainAmmount()
{
	int rain = 0;
	rain = 11 * _rainCounter;
	_rainCounter = 0;
	return rain;
} 

//Updates the wind direction internal state.
int ADSWeather::_readWindDir()
{
	unsigned int maximum, sum;
	unsigned char i, j, max_i;
	
	//Clear wind vane averaging bins
	for(i=0;i<16;i++)
	{
		_windDirBin[i] = 0;
	}
	
	//Read all samples into bins
	for(i=0;i<50;i++)
	{
		_setBin(_vaneSample[i]);
	}
	
	//Calculate the weighted average
	//Find the blokc of 5 bins with the highest sum
	maximum = 0;
	for(i=0;i<16;i++)
	{
		//get the sum of the next 5 bins
		sum = 0;
		for(j=0;j<5;j++)
		{
			sum += _windDirBin[(i+j) & 0x0F];
		}
		if(sum > maximum)
		{
			maximum = sum;
			max_i = i;
		}
	}
	sum = 0;
	for(i=1;i<5;i++)
	{
		sum += (_windDirBin[(max_i + i) & 0x0F] * i);
	}
	sum = ((max_i * 45) + ((sum * 45) / maximum) >> 1) % 360; //Convert into degrees
	return sum;
}


//returns the wind speed since the last calcInterval.
int ADSWeather::_readWindSpd()
{
	unsigned char i;
	
	long spd = 14920;
	spd *= _anemometerCounter;
	spd /= 1000;
	_anemometerCounter = 0;
	if(_gustIdx > 29)
	{
		_gustIdx = 0;
	}
	_gust[_gustIdx] = (int) spd;

	for (i = 0; i < 30; i++)
	{
		if (_gust[i] > _windSpdMax) _windSpdMax = _gust[i];	
	}
	return (int) spd;
}

//Internal function for calculatin the wind direction using consensus averaging.  
void ADSWeather::_setBin(unsigned int windVane)
{
	//Read wind directions into bins
	unsigned char bin;
	if( windVane > 940) bin = 12;     //W
	else if(windVane > 890) bin = 14; //NW
	else if(windVane > 820) bin = 13; //WNW
	else if(windVane > 785) bin = 0;  //N
	else if(windVane > 690) bin = 15; //NNW
	else if(windVane > 630) bin = 10; //SW
	else if(windVane > 590) bin = 11; //WSW
	else if(windVane > 455) bin = 2;  //NE
	else if(windVane > 400) bin = 1;  //NNE
	else if(windVane > 285) bin = 8;  //S
	else if(windVane > 240) bin = 9;  //SSW
	else if(windVane > 180) bin = 6;  //SE
	else if(windVane > 125) bin = 7;  //SSE
	else if(windVane > 90)  bin = 4;  //E
	else if(windVane > 80)  bin = 3;  //ESE
	else bin = 5;
	_windDirBin[bin]++;
}

//ISR for rain gauge.
static void ADSWeather::countRain()
{

	if((long)(micros() - last_micros_rg) >= DEBOUNCE_TIME * 1000)
	{
		
		_rainCounter++;
		last_micros_rg = micros();
	}
	
}

//ISR for anemometer.
static void ADSWeather::countAnemometer()
{
	if((long)(micros() - last_micros_an) >= DEBOUNCE_TIME * 1000)
	{
		_anemometerCounter++;
		last_micros_an = micros();
	}
}
