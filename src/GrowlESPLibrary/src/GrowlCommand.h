#pragma once
#include <string>
#define ARDUINOJSON_ENABLE_STD_STRING 1
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
class GrowlCommand
{
public:
	GrowlCommand(std::string name, int value);
	JsonObject toJSON();
private:
	int _targetId = -1;
	std::string _name;
	int _value;
};

