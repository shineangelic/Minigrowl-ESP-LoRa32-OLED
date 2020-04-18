#include <GrowlDevice.h>
#include <GrowlActuator.h>

#define ARDUINOJSON_ENABLE_STD_STRING 1
#include <ArduinoJson.h>
using namespace ARDUINOJSON_NAMESPACE;

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
	doc["typ"]	= _name;
	doc["val"]	= _reading;
	doc["id"]	= _gpioid;
	doc["err"]	= _errorPresent;
	doc["mode"] = _mode;

	// array dei comandi supportati
	JsonArray supportedCommandsJA = doc.createNestedArray("cmds");

	std::vector<GrowlCommand*>::iterator iter, end;
	/*for (iter = _supportedCommands.begin(), end = _supportedCommands.end(); iter != end; ++iter) {
		JsonObject ret = supportedCommandsJA.createNestedObject();
		(*iter)->toJSON(&ret);
	}*/
	for (auto const& value : _supportedCommands) {
		JsonObject ret = supportedCommandsJA.createNestedObject();
		value->toJSON(&ret);
	}


	std::string s("");
	int p = serializeJson(doc, s);
	//Serial.print("ACTUATOR SERIALIZED: ");
	//Serial.println(s.c_str());

	return s;
}

short GrowlActuator::getMode()
{
	return _mode;
}

int GrowlActuator::executeCommand(GrowlCommand exec)
{
	if (exec.getTargetActuatorId() != this->getPid()) {
		Serial.print("COMMAND EXECUTION ERROR PID MISMATCH WAS ");
		Serial.print(exec.getTargetActuatorId());
		Serial.print(" BUT EXPECTED ");
		Serial.print(this->getPid());
		return -1;
	}
	if (exec.getValueParameter() == MODE_AUTO || exec.getValueParameter() == MODE_MANUAL) {
		_mode = exec.getValueParameter();
		return 1; //mode changed
	}
	if (this->getMode() == MODE_AUTO) {
		Serial.println("COMMAND NOT EXECUTED, AUTO MODE");
		return -1;
	}
	Serial.print("COMMAND EXEC WRITING PID: ");
	Serial.print(exec.getTargetActuatorId());
	Serial.print(" VAL: ");
	Serial.println(exec.getValueParameter());
	//digitalWrite(exec.getTargetActuatorId(), exec.getValueParameter());
	_reading = exec.getValueParameter();
	//Serial.println(bitRead(PORTD, 3));
}
