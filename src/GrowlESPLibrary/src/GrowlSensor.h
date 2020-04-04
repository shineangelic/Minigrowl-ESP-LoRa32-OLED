#pragma once
#include <ctime>
#include <iostream>
class GrowlDevice
{

public:
	void setReading(float newVal);
	float getReading();
	GrowlDevice(int gpid);
	GrowlDevice();
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

class LightSensor : public GrowlDevice {
public:
	LightSensor(int gpid) : GrowlDevice(gpid) {
		_sensortype = 'L';
	};
	LightSensor() : GrowlDevice() {
		_sensortype = 'L';
	};
};

class TemperatureSensor : public GrowlDevice {
public:
	TemperatureSensor(int gpid) : GrowlDevice(gpid) {
		_sensortype = 'T';
	};
	TemperatureSensor() : GrowlDevice() {
		_sensortype = 'T';
	};
};

class HumiditySensor : public GrowlDevice {
public:
	HumiditySensor(int gpid) : GrowlDevice(gpid) {
		_sensortype = 'H';
	};
	HumiditySensor() : GrowlDevice() {
		_sensortype = 'H';
	};
};
