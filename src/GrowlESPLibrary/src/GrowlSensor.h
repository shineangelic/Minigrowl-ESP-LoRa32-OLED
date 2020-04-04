#pragma once
#include <ctime>
#include <iostream>
class GrowlSensor
{

public:
	void setReading(float newVal);
	float getReading();
	GrowlSensor(int gpid);
	std::string toJSON();
	int getPid();
	void setPid(int pid);
protected:
	int			_gpioid;
	char		_sensortype;
	float		_reading;
	char		_unit;
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
