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
 
	MainLights() : GrowlActuator() {
		_name = "Main Lights";
		_supportedCommands.push_back(new GrowlCommand("Switch lights ON", 1));
		_supportedCommands.push_back(new GrowlCommand("Switch lights OFF", 0));
	};
};

class OutTakeFan : public GrowlActuator {
public:

	OutTakeFan() : GrowlActuator() {
		_name = "Fan";
		_supportedCommands.push_back(new GrowlCommand("Turn ON outtake Fan", 1));
		_supportedCommands.push_back(new GrowlCommand("Turn OFF outtake Fan", 0));
	};
};

class IntakeFan : public GrowlActuator {
public:

	IntakeFan() : GrowlActuator() {
		_name = "IntakeFan";
		_supportedCommands.push_back(new GrowlCommand("Turn ON intake Fan", 1));
		_supportedCommands.push_back(new GrowlCommand("Turn intake Fan OFF", 0));
	};
};

class Hvac : public GrowlActuator {
public:
 
	Hvac() : GrowlActuator() {
		_name = "Heat&Conditioning";
		_supportedCommands.push_back(new GrowlCommand("Set Temperature", 1));
		_supportedCommands.push_back(new GrowlCommand("Turn OFF Hvac", 0));
	};
};

