#include <GrowlDevice.h>
#include <GrowlSensor.h>
#include <iostream>
#include <string>

#define ARDUINOJSON_ENABLE_STD_STRING 1
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
using namespace ARDUINOJSON_NAMESPACE;


GrowlSensor::GrowlSensor(int bid,int gpid)
{
	_gpioid = gpid;
	_boardId = bid;
}

GrowlSensor::GrowlSensor(int bid)
{
	_boardId = bid;
}

std::string GrowlSensor::toJSON()
{

	DynamicJsonDocument doc(600);

	// Add values in the document
	std::string t(1, _sensortype);
	doc["typ"] = t;
	doc["val"] = _reading;
	doc["id"] = _gpioid;
	doc["bid"] = _boardId;
	doc["err"] = _errorPresent;
	doc["unt"] = _unit;

	std::string s("");
	serializeJson(doc, s);

	return s;
}



