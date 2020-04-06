
#include <WiFi.h>
#include <HTTPClient.h>
#include <iostream>
#include <string>
#include <ostream>

#include "GrowlManager.h"
//used to define "same GPIO" sensors (tipo DHT)
#define MAXPINS 32


const char* host = "192.168.0.54:8080";

//LightSensor		_lightSensor(LIGHT_SENSOR);
//HumiditySensor  _humidity(DHTPIN);

//the pin has to be already set
void GrowlManager::initChamber()
{
	_pc = 0;
	_chamber.init();
	//_chamber.setLightSensorPin(LIGHT_SENSOR);

	//_humiditySensor.setPid(_chamber.getHimiditySensorPin());
	//_tempSensor.setPid(_chamber.getTempSensorPin());
	//sendCurrentSensors();
	_sensorsPtr.push_back(&_lightSensor);
	_sensorsPtr.push_back(&_tempSensor);
	_sensorsPtr.push_back(&_humiditySensor);


	_actuatorsPtr.push_back(&_inTakeFan);
	_actuatorsPtr.push_back(&_outTakeFan);
	_actuatorsPtr.push_back(&_mainLights);
	_actuatorsPtr.push_back(&_hvac);
}

void GrowlManager::loop()
{

	_pc++;
	_chamber.switchHeater(true);

	_chamber.switchMainLights(true);

	//TEST SPINTA
	if (_pc % 2) {
		_chamber.switchIntakeFan(true);
		_chamber.switchOuttakeFan(true);
	}
	else {
		//_chamber.switchIntakeFan(false);
		//_chamber.switchOuttakeFan(false);
	}
	//LIFECYCLE

	//hw readings
	float luxR = _chamber.getLumen();
	float hum = _chamber.getHumidity();
	float temp = _chamber.getTemperature();
	//std::ostringstream ss;
	//ss << luxR;
	//std::string s(ss.str());
	_lightSensor.setReading(luxR);
	_humiditySensor.setReading(hum);
	_tempSensor.setReading(temp);

	_mainLights.setReading(_chamber.getMainLightsStatus());
	_outTakeFan.setReading(_chamber.getOuttakeFanStatus());
	_inTakeFan.setReading(_chamber.getIntakeFanStatus());
	_hvac.setReading(_chamber.getHeatingStatus());

	//retrieveServerCommands();

	//applyServerCommands();

	_chamber.loop();//yield() alla fine


	sendCurrentSensors();
	delay(500);
	sendActuators();
	 



}

void GrowlManager::sendCurrentSensors()
{
	WiFiClient client;
	HTTPClient http;
	/*const int httpPort = 8080;
	if (!client.connect(host, httpPort)) {
		Serial.println("connection failed");
		return;
	}*/
	GrowlSensor* toSend = _sensorsPtr.at(_pc % _sensorsPtr.size());
	// We now create a URI for the request
	String url = "/api/esp/v1/sensors/id/";
	url += toSend->getPid();

	Serial.print("Send sensor: ");
	Serial.println(url);

	// This will send the request to the server
	/*client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
		"Host: " + host + "\r\n" +
		"Connection: close\r\n\r\n");*/


	Serial.print("HTTP REQ:");
	Serial.println(toSend->toJSON().c_str());

	http.begin(String("http://") + host + url); //Specify destination for HTTP request
	http.addHeader("Content-Type", "application/json;charset=UTF-8"); //Specify content-type header
	int httpResponseCode = http.PUT(toSend->toJSON().c_str()); //Send the actual POST request

	Serial.print("HTTP RESPONSE:");
	Serial.println(httpResponseCode);
	//Serial.println(_lightSensor.toJSON().c_str());

}

void GrowlManager::sendActuators()
{
	WiFiClient client;
	HTTPClient http;
	GrowlActuator* toSend = _actuatorsPtr.at(_pc % _sensorsPtr.size());
	/*const int httpPort = 8080;
	if (!client.connect(host, httpPort)) {
		Serial.println("connection failed");
		return;
	}*/

	// We now create a URI for the request
	String url = "/api/esp/v1/actuators/id/";
	url += toSend->getPid();

	Serial.print("Send actuator: ");
	Serial.println(url);

	Serial.print("HTTP REQ:");
	try {
		Serial.println(toSend->toJSON().c_str());
	}
	catch (int e) {
		Serial.print("ERRORE GATTICO An exception occurred. Exception Nr. " + e);
	}


	http.begin(String("http://") + host + url); //Specify destination for HTTP request
	http.addHeader("Content-Type", "application/json;charset=UTF-8"); //Specify content-type header
	int httpResponseCode = http.PUT(toSend->toJSON().c_str()); //Send the actual POST request

	Serial.print("HTTP RESPONSE:");
	Serial.println(httpResponseCode);
}


std::string GrowlManager::reportStatus()
{
	std::ostringstream ss;
	ss << "LIGHT: ";
	ss << (_mainLights.getReading() != 0 ? "ON" : "OFF");
	ss << " HEATER: ";
	ss << (_hvac.getReading() != 0 ? "ON" : "OFF");
	ss << "\n";
	ss << "FAN: IN: ";
	ss << (_inTakeFan.getReading() != 0 ? "ON" : "OFF");
	ss << " - OUT: ";
	ss << (_outTakeFan.getReading() != 0 ? "ON" : "OFF");
	ss << "\n";
	ss << "Hum: ";
	ss << this->getChamber().getHumidity() << "%";
	ss << " Temp: ";
	ss << this->getChamber().getTemperature() << "°C";
	ss << "\n";
	ss << "Lumen: ";
	ss << this->getChamber().getLumen() << "lux";
	//std::string s(ss.str());
	return ss.str();
	//return std::to_string(_chamber.getHumidity()) << std::to_string(_chamber.getTemperature());
}

std::tm GrowlManager::getGrowlManagerTime() {
	std::tm t = {};
	return t;
}

GrowlChamber GrowlManager::getChamber()
{
	return _chamber;
}

void GrowlManager::initMainLights(int HWPIN)
{
	_chamber.setMainLightsPin(HWPIN);
	_mainLights.setPid(HWPIN);
}

void GrowlManager::initIntakeFan(int HWPIN)
{
	_chamber.setIntakeFanPin(HWPIN);
	_inTakeFan.setPid(HWPIN);
}

void GrowlManager::initOuttakeFanPin(int HWPIN)
{
	_chamber.setOuttakeFanPin(HWPIN);
	_outTakeFan.setPid(HWPIN);
}

void GrowlManager::setHeaterPin(int HWPIN)
{
	_chamber.setHeaterPin(HWPIN);
	_hvac.setPid(HWPIN);
}

void GrowlManager::initLightSensor(int HWPIN)
{
	_lightSensor.setPid(HWPIN);
	_chamber.setLightSensorPin(HWPIN);
}

void GrowlManager::setDhtPin(int HWPIN)
{
	_tempSensor.setPid(HWPIN);
	_chamber.setDhtPin(HWPIN);
	_humiditySensor.setPid(HWPIN + MAXPINS);//received in constructor
	//_humiditySensor.setPid(_dht_pin + MAX_SENSORS);//received in constructor
}

