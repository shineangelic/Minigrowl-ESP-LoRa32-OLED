#pragma once
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <DHT_U.h>
#include <DHT.h>

#include <GrowlSensor.h>

#define DHTPIN 22 
#define DHTTYPE DHT22

class GrowlChamber
{

public:
	GrowlChamber();
	void init();
	void loop();
	bool getMainLightsStatus();
	bool getIntakeFanStatus();
	bool getOuttakeFanStatus();
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
	float getTemperature();
	float getHumidity();
	float getLumen();
private:
	int _mainLightPIN;
	int _intakePIN;
	int _outtakePIN;
	int _heaterPIN;
	int _lightSensorPIN;

	bool _isMainLightsON;
	bool _isIntakeFanON;
	bool _isOuttakeON;
	bool _isHeaterON;
	//LightSensor _lightSensor(LIGHT_SENSOR);
};

