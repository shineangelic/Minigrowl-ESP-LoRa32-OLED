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
#include <Chamber.h>
#include <GrowlSensor.h>
#include <GrowlActuator.h>
/* A chamber represents a grow chamber, with two temperature&hum sensors and 4 commandable devices,
here named lights, outFan and inFan and Heater */


class GrowlChamber : public Chamber
{

public:
	GrowlChamber() {};
	GrowlChamber(int bid) : _outTakeFan(bid), _humiditySensor(bid),	_tempSensor(bid),
		_barometer(bid), _mainLights(bid),_hvac(bid), _inTakeFan(bid), _lightSensor(bid),
		_humiditySensorExt(bid), _tempSensorExt(bid){
 
	};
	void init();
	void loop(); 

	std::string reportStatus();

	bool switchMainLights(int on);
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

	bool hasErrors();
	MainLights*		getMainLights();
	IntakeFan*		getIntakeFan();
	OutTakeFan*		getOuttakeFan();
	Hvac*			getHeater();
	float			getPressure();
	int				getLumen();//NOT real Lumen!
	BaromenterSensor*	getBarometerSensor();
	TemperatureSensor*	getTemperatureSensor();
	HumiditySensor*		getHumiditySensor();
	TemperatureSensor*	getExternalTemperatureSensor();
	HumiditySensor*		getExternalHumiditySensor();
	LightSensor*		getLightSensor();
private:
	
	bool initTemp();
	static void tempTask(void* pvParameters);
	int _dht_pin;
	int _SCLPIN;
	int _SDAPIN;

	OutTakeFan				_outTakeFan;
	MainLights				_mainLights;
	Hvac					_hvac;
	IntakeFan				_inTakeFan;

	LightSensor					_lightSensor;
	HumiditySensor				_humiditySensor;
	TemperatureSensor			_tempSensor;
	HumiditySensor				_humiditySensorExt;
	TemperatureSensor			_tempSensorExt;
	BaromenterSensor			_barometer;
};

