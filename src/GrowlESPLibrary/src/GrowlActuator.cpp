#include "GrowlActuator.h"
#define ARDUINOJSON_ENABLE_STD_STRING 1
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
using namespace ARDUINOJSON_NAMESPACE;

GrowlActuator::GrowlActuator(int gpid)
{
	_gpioid = gpid;
}

GrowlActuator::GrowlActuator()
{
}

std::string GrowlActuator::toJSON()
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
	// 
	doc["name"] = _name;
	doc["val"] = _reading;
	doc["id"] = _gpioid;
	doc["err"] = _errorPresent;

	// create an empty array
	JsonArray array = doc.createNestedArray("cmds");

	std::vector<GrowlCommand*>::iterator iter, end;
	for (iter = _supportedCommands.begin(), end = _supportedCommands.end(); iter != end; ++iter) {
		//std::cout << (*iter)->display_card() << std::endl;
		array.add((*iter)->toJSON());
	}

	

	// Add an array.
	//
	/*JsonArray data = doc.createNestedArray("data");
	data.add(48.756080);
	data.add(2.302038);*/

	std::string s("");
	int p = serializeJson(doc, s);

	return s;
}