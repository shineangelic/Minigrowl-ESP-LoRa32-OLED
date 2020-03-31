#include <sstream>
#include <string>

#include "GrowlManager.h"
GrowlManager::GrowlManager()
{
	_chamber = GrowlChamber();
}

void GrowlManager::init()
{
	_pc = 0;
	_chamber.init();
	
}

void GrowlManager::loop()
{
	_pc++;
	_chamber.switchMainLights(false);
	delay(200);
	_chamber.switchMainLights(true);
}

std::string GrowlManager::reportStatus()
{	 
	std::ostringstream ss;
	ss << "Humidity: ";
	ss << this->getChamber().getHumidity() << "%";
	ss << " - Temp: ";
	ss << this->getChamber().getTemperature() << "%";
	ss << endl;
	std::string s(ss.str());
	return s;
	//return std::to_string(_chamber.getHumidity()) << std::to_string(_chamber.getTemperature());
}

std::tm GrowlManager::getGrowlManagerTime() {
	std::tm t = {};
	return t;
}

GrowlChamber GrowlManager::getChamber()
{
	return _chamber;
}

void GrowlManager::setMainLightsPin(int HWPIN)
{
	_chamber.setMainLightsPin(HWPIN);
}
