#pragma once
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif 
//Libraries for BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Chamber.h>
#include <GrowlSensor.h>
#include <GrowlActuator.h>
/* A chamber represents a grow chamber, with two temperature&hum sensors and 4 commandable devices,
here named lights, outFan and inFan and Heater */


class DryChamber : public Chamber
{

public:
	DryChamber(){};
	void init();
	void loop(); 
	std::string reportStatus(); 
	bool switchIntakeFan(bool on);
	bool switchOuttakeFan(bool on);  

	void setMainLightsPin(int HWPIN);
	void setIntakeFanPin(int HWPIN);
	void setOuttakeFanPin(int HWPIN);
	void setHeaterPin(int HWPIN);
	void setLightSensorPin(int HWPIN);
	void setDhtPin(int HWPIN);
	void setBME280Pin(int SCLPIN, int SDAPIN);

	bool hasErrors(); 
	IntakeFan*		getIntakeFan();
	OutTakeFan*		getOuttakeFan();
	float			getPressure();
	int				getLumen();//NOT real Lumen!
	BaromenterSensor*	getBarometerSensor();
	TemperatureSensor*	getTemperatureSensor();
	HumiditySensor*		getHumiditySensor();
	HumiditySensor*		getExternalHumiditySensor(); 
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
	BaromenterSensor			_barometer;
};

