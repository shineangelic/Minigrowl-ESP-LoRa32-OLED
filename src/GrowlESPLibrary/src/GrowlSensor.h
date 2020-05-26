#pragma once
#include <ctime>
#include <iostream>
#include <GrowlDevice.h>
class GrowlSensor : public GrowlDevice
{

public:
	GrowlSensor(int bid,int gpid);
	GrowlSensor(int bid);
	GrowlSensor() {
		_boardId = -1;
	};
	std::string toJSON();

protected:
	char		_sensortype;
	char		_unit;

};

class LightSensor : public GrowlSensor {
public:
	LightSensor(int bid, int gpid) : GrowlSensor(bid,gpid) {
		_sensortype = 'L';
	};
	LightSensor(int bid) : GrowlSensor(bid) {
		_sensortype = 'L';
	};
	LightSensor() : GrowlSensor() {
		_sensortype = 'L';
	};
};

class TemperatureSensor : public GrowlSensor {
public:
	TemperatureSensor(int bid, int gpid) : GrowlSensor(bid, gpid) {
		_sensortype = 'T';
	};
	TemperatureSensor(int bid) : GrowlSensor(bid) {
		_sensortype = 'T';
	};
	TemperatureSensor() : GrowlSensor() {
		_sensortype = 'T';
	};
};

class HumiditySensor : public GrowlSensor {
public:
	HumiditySensor(int bid) : GrowlSensor(bid) {
		_sensortype = 'H';
	};
	HumiditySensor() : GrowlSensor() {
		_sensortype = 'H';
	};
};
class BaromenterSensor : public GrowlSensor {
public:
	BaromenterSensor(int bid) : GrowlSensor(bid) {
		_sensortype = 'B';
	};
	BaromenterSensor() : GrowlSensor() {
		_sensortype = 'B';
	};
};


