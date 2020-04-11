#pragma once
#include <string>
#define ARDUINOJSON_ENABLE_STD_STRING 1
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
class GrowlCommand
{
public:
	GrowlCommand(std::string name, int value);
	GrowlCommand(std::string name,int value,int targetPid, int queueId);
	GrowlCommand(JsonVariant in);
	std::string toJSONstr();
	JsonObject toJSON();
	
	int getTargetActuatorId();
	int getValueParameter();
	int getQueueId();
private:
	int _targetId = -1;
	std::string _name;
	int _value;
	int _queueId;//nullable not serialized
};

