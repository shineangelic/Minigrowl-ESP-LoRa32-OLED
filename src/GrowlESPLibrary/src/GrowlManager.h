#pragma once

#include <sstream>
#include <string>
#include <vector>

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <GrowlSensor.h>
#include <GrowlActuator.h>
#include <ctime>
#include <GrowlChamber.h>

class GrowlManager
{
public:
	GrowlManager() : _chamber() {

	};
	void initChamber();
	void loop();
	std::string reportStatus();
	std::tm	getGrowlManagerTime();
	GrowlChamber getChamber();
	void setMainLightsPin(int HWPIN);
	void setIntakeFanPin(int HWPIN);
	void setOuttakeFanPin(int HWPIN);
	void setHeaterPin(int HWPIN);
	void setLightSensorPin(int HWPIN);
	void setDhtPin(int HWPIN);
private:
	void sendCurrentSensors();
	void sendActuators();
	short _pc;//program counter
	GrowlChamber	_chamber;
	std::tm			_time;
	std::vector<GrowlSensor*> _sensorsPtr;
	std::vector<GrowlActuator*> _actuatorsPtr;
	LightSensor		_lightSensor;
	HumiditySensor  _humiditySensor;
	TemperatureSensor _tempSensor;
	MainLights _mainLights;
	Fan		_inTakeFan;
	Fan		_outTakeFan;
	int _dht_pin;
};

