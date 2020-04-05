#include "GrowlSensor.h"
#include <iostream>
#include <string>

#define ARDUINOJSON_ENABLE_STD_STRING 1
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
using namespace ARDUINOJSON_NAMESPACE;


GrowlSensor::GrowlSensor(int gpid)
{
	_gpioid = gpid;
}

GrowlSensor::GrowlSensor()
{
}

std::string GrowlSensor::toJSON()
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
	std::string t(1, _sensortype);
	doc["typ"] = t;
	doc["val"] = _reading;
	doc["id"] = _gpioid;
	doc["err"] = _errorPresent;
	doc["unt"] = _unit;

	// Add an array.
	//
	/*JsonArray data = doc.createNestedArray("data");
	data.add(48.756080);
	data.add(2.302038);*/

	std::string s("");
	serializeJson(doc, s);

	return s;
}



