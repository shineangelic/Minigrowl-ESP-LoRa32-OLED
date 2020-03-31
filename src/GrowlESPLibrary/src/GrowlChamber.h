#pragma once
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <DHT_U.h>
#include <DHT.h>



class GrowlChamber
{

public:
	GrowlChamber();
	void init();
	void loop();
	bool switchMainLights(bool on);
	void setMainLightsPin(int HWPIN);
	float getTemperature();
	float getHumidity();
private:
	int _mainLightPIN;
	bool _isMainLightsON;
};

