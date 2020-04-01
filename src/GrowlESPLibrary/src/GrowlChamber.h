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
#define LIGHT_SENSOR 33


class GrowlChamber
{

public:
	GrowlChamber();
	void init();
	void loop();
	bool getMainLightsStatus();
	bool switchMainLights(bool on);
	void setMainLightsPin(int HWPIN);
	float getTemperature();
	float getHumidity();
	float getLumen();
private:
	int _mainLightPIN;
	bool _isMainLightsON;
	//LightSensor _lightSensor(LIGHT_SENSOR);
};

