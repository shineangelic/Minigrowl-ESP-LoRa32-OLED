#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <time.h>
#include <iostream> 
#include <queue>
#include <deque>
#include <array> // for array, at() 
#include <tuple> // for get() 

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <GrowlSensor.h>
#include <GrowlActuator.h>
#include <ctime>
#include <GrowlChamber.h>
#include <DryChamber.h>



class GrowlManager
{
public:
	GrowlManager(int bid, int mode) {
		_boardId = bid; _chamberMode = mode;
		if (_chamberMode == 1) {
			_chamberM = GrowlChamber(bid);
			_chamber = (Chamber*)&_chamberM;
		}
		else {
			_chamberD = DryChamber(bid);
			_chamber = (Chamber*)&_chamberD;
		}
	};
	void initChamber(const char* host, const int port, const char* prot);
	void loop();
	void calcDelay();
	std::string reportStatus();
	Chamber* getChamber();
	void initMainLights(int HWPIN);
	void initIntakeFan(int HWPIN);
	void initOuttakeFanPin(int HWPIN);
	bool hasChamberError();
	void setHeaterPin(int HWPIN);
	void initLightSensor(int HWPIN);
	void setDhtPin(int HWPIN);
	void setBME280Pin(int SCLPIN, int SDAPIN);
private:
	void applyServerCommands();
	bool removeExecutedCommand(GrowlCommand* executed);
	void retrieveServerCommands();
	void sendRandomSensor();
	void sendSensor(String completeUrl, GrowlSensor* toSend);
	void sendActuator(const String completeUrl, GrowlActuator* toSend);
	void sendRandomActuator();
	void chamberLogic();
	short _pc;//program counter
	short _sburtoMode;
	Chamber*	_chamber;
	GrowlChamber _chamberM;
	DryChamber _chamberD;
	std::vector<GrowlSensor*>	_sensorsPtr;
	std::vector<GrowlActuator*> _actuatorsPtr;
	const char* _host;//"192.168.0.54";
	const char* _proto = "http://";
	int _httpPort;
	int _boardId;
	int _chamberMode;//1 grow - 2 dry
	//commands to be executed
	std::deque<GrowlCommand> _commandsQueue;
};

