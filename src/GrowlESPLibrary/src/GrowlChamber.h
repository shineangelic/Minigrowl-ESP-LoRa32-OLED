#pragma once
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <DHTesp.h>
//Libraries for BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <GrowlSensor.h>
#include <GrowlActuator.h>

class GrowlChamber
{

public:
	GrowlChamber() {};
	void init();
	void loop();
	bool getMainLightsStatus();
	bool getIntakeFanStatus();
	bool getHeatingStatus();
	bool switchMainLights(bool on);
	bool switchIntakeFan(bool on);
	bool switchOuttakeFan(bool on);
	bool switchHeater(bool on);
	void setMainLightsPin(int HWPIN);
	void setIntakeFanPin(int HWPIN);
	void setOuttakeFanPin(int HWPIN);
	void setHeaterPin(int HWPIN);
	void setLightSensorPin(int HWPIN);
	void setDhtPin(int HWPIN);
	void setBME280Pin(int SCLPIN, int SDAPIN);
	MainLights* getMainLights();
	IntakeFan* getIntakeFan();
	OutTakeFan* getOuttakeFan();
	Hvac* getHeater();
	float getTemperature();
	float getHumidity();
	float getPressure();
	int getLumen();//NOT real Lumen!
	BaromenterSensor* getBarometerSensor();
	TemperatureSensor* getTemperatureSensor();
	HumiditySensor* getHumiditySensor();
	LightSensor* getLightSensor();
private:
	
	bool initTemp();
	int _dht_pin;
	int _SCLPIN;
	int _SDAPIN;

	OutTakeFan				_outTakeFan;
	MainLights				_mainLights;
	Hvac					_hvac;
	IntakeFan				_inTakeFan;

	LightSensor				_lightSensor;
	HumiditySensor			_humiditySensor;
	TemperatureSensor		 _tempSensor;
	BaromenterSensor		_barometer;
};

