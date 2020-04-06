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


JsonObject  GrowlCommand::toJSON()
{
	// Allocate the JSON document
	//
	// Inside the brackets, 200 is the RAM allocated to this document.
	// Don't forget to change this value to match your requirement.
	// Use arduinojson.org/v6/assistant to compute the capacity.
	StaticJsonDocument<200> doc;

	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);

	// Add values in the document
	
	doc["name"] = _name;
	doc["val"] = _value;

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

