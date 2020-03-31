#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <ctime>
#include "GrowlChamber.h"

class GrowlManager
{
public:
	GrowlManager();
	void init();
	void loop();
	std::string reportStatus();

	std::tm	getGrowlManagerTime();
	GrowlChamber getChamber();
	void setMainLightsPin(int HWPIN);
private:
	short _pc;//program counter
	GrowlChamber _chamber;
	std::tm _time;
};

