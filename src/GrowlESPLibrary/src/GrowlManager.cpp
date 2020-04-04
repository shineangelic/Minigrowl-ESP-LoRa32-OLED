
#include <WiFi.h>
#include <HTTPClient.h>
#include <iostream>
#include <string>
#include <ostream>

#include "GrowlManager.h"
#define LIGHT_SENSOR 33
//#define DHTPIN 22 
//#define DHTTYPE DHT22

//LightSensor		_lightSensor(LIGHT_SENSOR);
//HumiditySensor  _humidity(DHTPIN);
 
//the pin has to be already set
void GrowlManager::initChamber()
{
	_pc = 0;
	_chamber.init();
	//_chamber.setLightSensorPin(LIGHT_SENSOR);
	_lightSensor.setPid(_chamber.getLightSensorPin());
	//_humiditySensor.setPid(_chamber.getHimiditySensorPin());
	//_tempSensor.setPid(_chamber.getTempSensorPin());
	//sendCurrentSensors();
	_devicesPtr.push_back(&_lightSensor);
	_devicesPtr.push_back(&_tempSensor);
	_devicesPtr.push_back(&_humiditySensor);
}

void GrowlManager::loop()
{
	_pc++;
	_chamber.switchHeater(true);
	_chamber.switchIntakeFan(true);
	_chamber.switchOuttakeFan(true);
	delay(200);
	_chamber.switchMainLights(true);

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

	//retrieveServerCommands();

	//applyServerCommands();

	sendCurrentSensors();

	_chamber.loop();
}

std::string GrowlManager::reportStatus()
{
	std::ostringstream ss;
	ss << "LIGHT: ";
	ss << (this->getChamber().getMainLightsStatus() ? "ON" : "OFF");
	ss << " HEATER: ";
	ss << (this->getChamber().getHeatingStatus() ? "ON" : "OFF");
	ss << "\n";
	ss << "FAN: IN: ";
	ss << (this->getChamber().getIntakeFanStatus() ? "ON" : "OFF");
	ss << " - OUT: ";
	ss << (this->getChamber().getOuttakeFanStatus() ? "ON" : "OFF");
	ss << "\n";
	ss << "Hum: ";
	ss << this->getChamber().getHumidity() << "%";
	ss << " Temp: ";
	ss << this->getChamber().getTemperature() << "°C";
	ss << "\n";
	ss << "Lumen: ";
	ss << this->getChamber().getLumen() << "lux";
	std::string s(ss.str());
	return s;
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

void GrowlManager::setMainLightsPin(int HWPIN)
{
	_chamber.setMainLightsPin(HWPIN);
}

void GrowlManager::setIntakeFanPin(int HWPIN)
{
	_chamber.setIntakeFanPin(HWPIN);
}

void GrowlManager::setOuttakeFanPin(int HWPIN)
{
	_chamber.setOuttakeFanPin(HWPIN);
}

void GrowlManager::setHeaterPin(int HWPIN)
{
	_chamber.setHeaterPin(HWPIN);
}

void GrowlManager::setLightSensorPin(int HWPIN)
{
	_lightSensor.setPid(HWPIN);
	_chamber.setLightSensorPin(HWPIN);
}

void GrowlManager::sendCurrentSensors()
{
	WiFiClient client;
	HTTPClient http;


	const char* host = "192.168.0.54:8080";
	/*const int httpPort = 8080;
	if (!client.connect(host, httpPort)) {
		Serial.println("connection failed");
		return;
	}*/

	// We now create a URI for the request
	String url = "/api/esp/v1/sensors/id/";
	url += _lightSensor.getPid();

	Serial.print("Requesting URL: ");
	Serial.println(url);

	// This will send the request to the server
	/*client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
		"Host: " + host + "\r\n" +
		"Connection: close\r\n\r\n");*/
	Serial.print("HTTP REQ:");
	Serial.println(_lightSensor.toJSON().c_str());
	http.begin(String("http://") + host + url); //Specify destination for HTTP request
	http.addHeader("Content-Type", "application/json;charset=UTF-8"); //Specify content-type header
	int httpResponseCode = http.PUT(_lightSensor.toJSON().c_str()); //Send the actual POST request

	Serial.print("HTTP RESPONSE:");
	Serial.println(httpResponseCode);
	//Serial.println(_lightSensor.toJSON().c_str());
}
