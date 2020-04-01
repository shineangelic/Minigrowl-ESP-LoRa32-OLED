#pragma once
#include <ctime>
#include <iostream>
class GrowlSensor
{

public:
	void setReading(std::string newVal);
	GrowlSensor(int gpid);
	std::string toJSON();
protected:
	int			_gpioid;
	char		_sensortype;
	std::string	_reading;
	char		_uinit;
	bool		_errorPresent;
};

class LightSensor : public GrowlSensor {
public:
	LightSensor(int gpid) : GrowlSensor(gpid) {
		_sensortype = 'L';
	};
};

class TemperatureSensor : public GrowlSensor {
public:
	TemperatureSensor(int gpid) : GrowlSensor(gpid) {
		_sensortype = 'T';
	};
};

class HumiditySensor : public GrowlSensor {
public:
	HumiditySensor(int gpid) : GrowlSensor(gpid) {
		_sensortype = 'H';
	};
};
