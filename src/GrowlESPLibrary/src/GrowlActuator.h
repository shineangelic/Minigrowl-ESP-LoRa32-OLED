#pragma once
#include "GrowlDevice.h"
#include "GrowlCommand.h"
#include <vector>

static const auto MODE_AUTO = -2;
static const auto  MODE_MANUAL = -1;

class GrowlActuator : public GrowlDevice
{
public:
	//GrowlActuator(int gpid);
	GrowlActuator() {
		_supportedCommands.push_back(new GrowlCommand("Manual mode", -1));
		_supportedCommands.push_back(new GrowlCommand("AUTO mode", -2));
	};
	std::string toJSON();
	short getMode();
	int executeCommand(GrowlCommand exec);
protected:
	std::string _name;
	short _mode = MODE_AUTO;//manual or auto
	std::vector<GrowlCommand*> _supportedCommands;
};

class MainLights : public GrowlActuator {
public:
	MainLights() : GrowlActuator() {
		_name = "L";
		_supportedCommands.push_back(new GrowlCommand("Switch lights ON", 1));
		_supportedCommands.push_back(new GrowlCommand("Switch lights OFF", 0));
	};
};

class OutTakeFan : public GrowlActuator {
public:

	OutTakeFan() : GrowlActuator() {
		_name = "F";
		_supportedCommands.push_back(new GrowlCommand("Turn ON Fan", 1));
		_supportedCommands.push_back(new GrowlCommand("Turn OFF Fan", 0));
	};
};

class IntakeFan : public GrowlActuator {
public:

	IntakeFan() : GrowlActuator() {
		_name = "F";
		_supportedCommands.push_back(new GrowlCommand("Turn ON intake Fan", 1));
		_supportedCommands.push_back(new GrowlCommand("Turn Fan OFF", 0));
	};
};

class Hvac : public GrowlActuator {
public:
 
	Hvac() : GrowlActuator() {
		_name = "H";
		_supportedCommands.push_back(new GrowlCommand("Turn ON Hvac", 1));
		_supportedCommands.push_back(new GrowlCommand("Set Temperature", 2));
		_supportedCommands.push_back(new GrowlCommand("Turn OFF Hvac", 0));
	};
};

class Humidifier : public GrowlActuator {
public:

	Humidifier() : GrowlActuator() {
		_name = "M";
		_supportedCommands.push_back(new GrowlCommand("Turn ON", 1));
		_supportedCommands.push_back(new GrowlCommand("Shut OFF threshold", 2));
		_supportedCommands.push_back(new GrowlCommand("Turn OFF", 0));
	};
};

