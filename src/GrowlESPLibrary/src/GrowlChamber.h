#pragma once
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
 
class GrowlChamber
{

public:
	bool switchMainLights(bool on);
	void setMainLightsPin(int HWPIN);
private:
	int _mainLightPIN;
	bool _isMainLightsON;
};

