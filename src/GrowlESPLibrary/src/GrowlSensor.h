#pragma once
#include <ctime>
#include <iostream>
#include "GrowlDevice.h"
class GrowlSensor: public GrowlDevice
{

public:
	GrowlSensor(int gpid);
	GrowlSensor();
	std::string toJSON();

protected:

	char		_sensortype;

	char		_unit;
	bool		_errorPresent;
};

class LightSensor : public GrowlSensor {
public:
	LightSensor(int gpid) : GrowlSensor(gpid) {
		_sensortype = 'L';
	};
	LightSensor() : GrowlSensor() {
		_sensortype = 'L';
	};
};

class TemperatureSensor : public GrowlSensor {
public:
	TemperatureSensor(int gpid) : GrowlSensor(gpid) {
		_sensortype = 'T';
	};
	TemperatureSensor() : GrowlSensor() {
		_sensortype = 'T';
	};
};

class HumiditySensor : public GrowlSensor {
public:
	HumiditySensor(int gpid) : GrowlSensor(gpid) {
		_sensortype = 'H';
	};
	HumiditySensor() : GrowlSensor() {
		_sensortype = 'H';
	};
};
