#include <sstream>
#include <string>
#include <WiFi.h>
#include <HTTPClient.h>

#include "GrowlManager.h"
#define LIGHT_SENSOR 33

LightSensor		_lightSensor(LIGHT_SENSOR);
HumiditySensor  _humidity(LIGHT_SENSOR);

GrowlManager::GrowlManager()
{
	_chamber = GrowlChamber();
}

void GrowlManager::init()
{
	_pc = 0;
	_chamber.init();
	_chamber.setLightSensorPin(LIGHT_SENSOR);
	//sendCurrentSensors();

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
	float luxR=_chamber.getLumen();
	float hum = _chamber.getHumidity();
	//std::ostringstream ss;
	//ss << luxR;
	//std::string s(ss.str());
	_lightSensor.setReading(luxR);
	_humidity.setReading(hum);

	//retrieveServerCommands();

	//applyServerCommands();

	sendCurrentSensors();
	

}

std::string GrowlManager::reportStatus()
{	 
	std::ostringstream ss;
	ss << "LIGHT: ";
	ss << this->getChamber().getMainLightsStatus();
	ss << " HEATER: ";
	ss << this->getChamber().getHeatingStatus();
	ss << "\n";
	ss << " IN FAN: ";
	ss << this->getChamber().getIntakeFanStatus();
	ss << " OUT FAN: ";
	ss << this->getChamber().getOuttakeFanStatus();
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
	http.begin(String("http://") + host  + url); //Specify destination for HTTP request
	http.addHeader("Content-Type", "application/json;charset=UTF-8"); //Specify content-type header
	int httpResponseCode = http.PUT(_lightSensor.toJSON().c_str()); //Send the actual POST request
	
	Serial.print("HTTP RESPONSE:");
	Serial.println(httpResponseCode);
	//Serial.println(_lightSensor.toJSON().c_str());
}
