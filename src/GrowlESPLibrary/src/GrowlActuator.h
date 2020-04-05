#pragma once
#include "GrowlDevice.h"
#include "GrowlCommand.h"
#include <vector>
class GrowlActuator : public GrowlDevice
{
public:
	GrowlActuator(int gpid);
	GrowlActuator();
	std::string toJSON();
protected:
	std::string _name;
	std::vector<GrowlCommand*> _supportedCommands;
};

class MainLights : public GrowlActuator {
public:
	MainLights(int gpid) : GrowlActuator(gpid) {
		_name = "Main Lights";
		_supportedCommands.push_back(new GrowlCommand("Turn ON", 1));
		_supportedCommands.push_back(new GrowlCommand("Turn OFF", 0));
	};
	MainLights() : GrowlActuator() {
		_name = "Main Lights";
		_supportedCommands.push_back(new GrowlCommand("Turn ON", 1));
		_supportedCommands.push_back(new GrowlCommand("Turn OFF", 0));
	};
};

class Fan : public GrowlActuator {
public:
	Fan(int gpid) : GrowlActuator(gpid) {
		_name = 'Fan';
		_supportedCommands.push_back(new GrowlCommand("Turn ON", 1));
		_supportedCommands.push_back(new GrowlCommand("Turn OFF", 0));
	};
	Fan() : GrowlActuator() {
		_name = 'Fan';
		_supportedCommands.push_back(new GrowlCommand("Turn ON", 1));
		_supportedCommands.push_back(new GrowlCommand("Turn OFF", 0));
	};
};

