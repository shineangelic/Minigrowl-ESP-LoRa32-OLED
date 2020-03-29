#pragma once
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
class GrowlChamber
{
	float _currentTemp;

public:
	float getCurrentTemp();
	GrowlChamber();
};

