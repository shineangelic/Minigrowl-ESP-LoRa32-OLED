#include <sstream>
#include <string>
#include <WiFi.h>
#include <HTTPClient.h>

#include "GrowlManager.h"

LightSensor _lightSensor(LIGHT_SENSOR);

GrowlManager::GrowlManager()
{
	_chamber = GrowlChamber();
}

void GrowlManager::init()
{
	_pc = 0;
	_chamber.init();
	//sendCurrentSensors();

}

void GrowlManager::loop()
{
	_pc++;
	_chamber.switchMainLights(false);
	delay(200);
	_chamber.switchMainLights(true);

	//LIFECYCLE
	float luxR=_chamber.getLumen();
	std::ostringstream ss;
	ss << luxR;
	std::string s(ss.str());
	_lightSensor.setReading(s);

	//retrieveServerCommands();

	//applyServerCommands();

	sendCurrentSensors();
	

}

std::string GrowlManager::reportStatus()
{	 
	std::ostringstream ss;
	ss << "LIGHT1: ";
	ss << this->getChamber().getMainLightsStatus();
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
	String url = "/putsensors/";


	Serial.print("Requesting URL: ");
	Serial.println(url);

	// This will send the request to the server
	/*client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
		"Host: " + host + "\r\n" +
		"Connection: close\r\n\r\n");*/
	http.begin(String("http://") + host  + url); //Specify destination for HTTP request
	http.addHeader("Content-Type", "text/plain"); //Specify content-type header
	int httpResponseCode = http.PUT(_lightSensor.toJSON().c_str()); //Send the actual POST request
	
	Serial.print("HTTP RESPONSE:");
	Serial.println(httpResponseCode);
	//Serial.println(_lightSensor.toJSON().c_str());
}
