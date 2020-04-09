#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <time.h>
#include<iostream> 
#include<array> // for array, at() 
#include<tuple> // for get() 

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <GrowlSensor.h>
#include <GrowlActuator.h>
#include <ctime>
#include <GrowlChamber.h>

using namespace std;

class GrowlManager
{
public:
	GrowlManager() : _chamber() {};
	void initChamber();
	void loop();
	void retrieveTime();
	std::string reportStatus();
	std::tm	getGrowlManagerTime();
	GrowlChamber getChamber();
	void initMainLights(int HWPIN);
	void initIntakeFan(int HWPIN);
	void initOuttakeFanPin(int HWPIN);
	void setHeaterPin(int HWPIN);
	void initLightSensor(int HWPIN);
	void setDhtPin(int HWPIN);
	void setBME280Pin(int SCLPIN, int SDAPIN);
private:
	void sendSensors();
	void sendActuators();
	void chamberLogic();
	short _pc;//program counter
	GrowlChamber	_chamber;
	//std::tm			_time;
	std::vector<GrowlSensor*> _sensorsPtr;
	std::vector<GrowlActuator*> _actuatorsPtr;
	LightSensor		_lightSensor;
	HumiditySensor  _humiditySensor;
	TemperatureSensor _tempSensor;
	MainLights _mainLights;
	Hvac	_hvac;
	IntakeFan		_inTakeFan;
	OutTakeFan		_outTakeFan;
};

