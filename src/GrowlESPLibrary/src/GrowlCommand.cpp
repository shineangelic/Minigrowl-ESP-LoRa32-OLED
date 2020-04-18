#include "GrowlCommand.h"

#define ARDUINOJSON_ENABLE_STD_STRING 1
#include <ArduinoJson.h>
using namespace ARDUINOJSON_NAMESPACE;

GrowlCommand::GrowlCommand(std::string name, int value)
{
	_name = name;
	_value = value;
	_targetId = -1;
	_queueId = -1;
}

GrowlCommand::GrowlCommand(std::string name, int value, int targetPid, int queueId)
{
	_name = name;
	_value = value;
	_targetId = targetPid;
	_queueId = queueId;
}

GrowlCommand::GrowlCommand(JsonVariant v)
{
	const char* charStr = (v["name"].as<char*>());
	std::string str(charStr);
	_queueId = (v["idOnQueue"].as<int>());
	_targetId = (v["tgt"].as<int>());
	_value = v["val"].as<int>();
	_name = str;

}

std::string  GrowlCommand::toJSONstr()
{

	DynamicJsonDocument doc(500);

	JsonObject ret = doc.createNestedObject();
	 toJSON(&ret);

	std::string s("");
	serializeJson(doc, s);
	return s;
}

//the json doc container must exists
void  GrowlCommand::toJSON(JsonObject* docref)
{
	JsonObject doc = *docref;//ref al padre
	
	doc["name"] = _name;
	doc["val"] = _value;
	if (_targetId != -1) {
		doc["tgt"] = _targetId;
	}
	if (_queueId != -1) {
	doc["idOnQueue"] = _queueId;
	}
}

int GrowlCommand::getTargetActuatorId()
{
	return _targetId;
}

int GrowlCommand::getValueParameter()
{
	return _value;
}

int GrowlCommand::getQueueId()
{
	return _queueId;
}

