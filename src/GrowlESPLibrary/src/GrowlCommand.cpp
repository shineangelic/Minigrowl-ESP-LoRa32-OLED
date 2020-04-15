#include "GrowlCommand.h"

#define ARDUINOJSON_ENABLE_STD_STRING 1
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
using namespace ARDUINOJSON_NAMESPACE;

GrowlCommand::GrowlCommand(std::string name, int value)
{
	_name = name;
	_value = value;
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
	//StaticJsonDocument<400> doc;
	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);
	doc = toJSON();

	std::string s("");
	serializeJson(doc, s);
	return s;
}


JsonObject  GrowlCommand::toJSON()
{

	//DynamicJsonDocument doc(500);
	StaticJsonDocument<400> doc;
	// TODO capire come mai dynamic resta vuoto

	doc["name"] = _name;
	doc["val"] = _value;
	doc["tgt"] = _targetId;
	if (_queueId != NULL)
		doc["idOnQueue"] = _queueId;
	// Add an array.
	//
	/*JsonArray data = doc.createNestedArray("data");
	data.add(48.756080);
	data.add(2.302038);*/

	/*std::string s("");
	serializeJson(doc, s);*/
	JsonObject ret = doc.to<JsonObject>();
	return ret;
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

