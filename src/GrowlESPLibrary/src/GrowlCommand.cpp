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
	// Allocate the JSON document
	//
	// Inside the brackets, 200 is the RAM allocated to this document.
	// Don't forget to change this value to match your requirement.
	// Use arduinojson.org/v6/assistant to compute the capacity.
	DynamicJsonDocument doc(1000);
	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);

	// Add values in the document

	doc["name"] = _name;
	doc["val"] = _value;
	doc["tgt"] = _targetId;
	//if (isNaN(_queueId))
	doc["idOnQueue"] = _queueId;

	// Add an array.
	//
	/*JsonArray data = doc.createNestedArray("data");
	data.add(48.756080);
	data.add(2.302038);*/

	std::string s("");
	serializeJson(doc, s);
	return s;
}


JsonObject  GrowlCommand::toJSON()
{
	// Allocate the JSON document
	//
	// Inside the brackets, 200 is the RAM allocated to this document.
	// Don't forget to change this value to match your requirement.
	// Use arduinojson.org/v6/assistant to compute the capacity.
	StaticJsonDocument<400> doc;

	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);

	// Add values in the document

	doc["name"] = _name;
	doc["val"] = _value;
	doc["tgt"] = _targetId;

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

