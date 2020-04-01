#include "GrowlSensor.h"

#define ARDUINOJSON_ENABLE_STD_STRING 1
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>

void GrowlSensor::setReading(std::string newVal)
{
	_reading = newVal;
}
GrowlSensor::GrowlSensor(int gpid)
{
	_gpioid = gpid;
}

std::string GrowlSensor::toJSON()
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
    //
    doc["Typ"] = _sensortype;
    doc["Val"] = _reading;
    doc["PID"] = _gpioid;
    doc["ERR"] = _errorPresent;

    // Add an array.
    //
    /*JsonArray data = doc.createNestedArray("data");
    data.add(48.756080);
    data.add(2.302038);*/
 
    std::string s("");
    serializeJson(doc, s);
    
    return s;
}
 
 