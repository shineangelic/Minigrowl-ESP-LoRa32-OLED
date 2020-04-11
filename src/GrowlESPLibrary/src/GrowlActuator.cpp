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
	DynamicJsonDocument doc(2000);

	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);

	// Add values in the document
	// 
	doc["typ"] = _name;
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


	std::string s("");
	int p = serializeJson(doc, s);

	return s;
}

int GrowlActuator::executeCommand(GrowlCommand exec)
{
	if (exec.getTargetActuatorId() != this->getPid()) {
		return -1;
		Serial.println("COMMAND EXECUTION ERROR PID MISMATCH");
	}
	Serial.print("COMMAND EXEC WRITING PID: ");
	Serial.print(exec.getTargetActuatorId());
	Serial.print(" VAL: ");
	Serial.println(exec.getValueParameter());
	//digitalWrite(exec.getTargetActuatorId(), exec.getValueParameter());
	_reading = exec.getValueParameter();
	//Serial.println(bitRead(PORTD, 3));
}
