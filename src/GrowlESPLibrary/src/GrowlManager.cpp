#include "GrowlManager.h"
GrowlManager::GrowlManager()
{

}

void GrowlManager::GrowlManagerInit()
{
	_pc = 0;
}

void GrowlManager::GrowlManagerLoop()
{
	_pc++;
	_chamber.switchMainLights(false);
	delay(200);
	_chamber.switchMainLights(true);
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
