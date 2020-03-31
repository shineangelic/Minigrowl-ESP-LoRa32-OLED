#pragma once
#include <ctime>
#include <iostream>
class Sensor
{

public:
	Sensor();
private:
	int id;
	std::string	 _type;
	std::string	 _reading;
	std::string	 _uinit;
	std::tm _timeStamp;
	bool _errorPresent;
};

