#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <GrowlSensor.h>
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
	void setIntakeFanPin(int HWPIN);
	void setOuttakeFanPin(int HWPIN);
	void setHeaterPin(int HWPIN);
	void setLightSensorPin(int HWPIN);
private:
	void sendCurrentSensors();
	short _pc;//program counter
	GrowlChamber _chamber;
	std::tm _time;

	
};

