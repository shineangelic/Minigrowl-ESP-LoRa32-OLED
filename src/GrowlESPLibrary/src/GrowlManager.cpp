
#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <iostream>
#include <string>
#include <sstream>
#include <ostream>

#include "GrowlManager.h"
 

using namespace std;

const char* host = "192.168.0.54:8080";
const float hourSchedule[] = { 1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

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

	//schedules and conditions
	chamberLogic();

	//retrieveServerCommands();

	//applyServerCommands();

	_chamber.loop();

	//sendCurrentSensors();
	delay(500);
	//sendActuators();
	retrieveTime();
}

void GrowlManager::chamberLogic()
{
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
	struct tm now;
	getLocalTime(&now, 0);

	//mainlights schedule
	_chamber.switchMainLights(hourSchedule[now.tm_hour]);

	//heat out take
	if (_chamber.getTemperature() > 29)
		_chamber.switchOuttakeFan(true);
	else
		_chamber.switchOuttakeFan(false);

}

void GrowlManager::retrieveTime()
{
	WiFiClient client;
	HTTPClient http;
	
	// We now create a URI for the request
	String url = "/api/esp/v1/getTime/";


	Serial.print("Retrieve time: ");
	Serial.println(url);

	Serial.print("HTTP REQ:");
	http.begin(String("http://") + host + url); //Specify destination for HTTP request
	http.addHeader("Content-Type", "application/json;charset=UTF-8"); //Specify content-type header
	int httpResponseCode = http.GET(); //Send the actual POST request

	Serial.print("HTTP RESPONSE:");
	Serial.println(httpResponseCode);
	if (httpResponseCode > 0) { //Check for the returning code

		unsetenv("TZ");
		String payload = http.getString();
		payload.replace("\"", "");
		Serial.print("Payload: ");
		Serial.println(payload);
		const char* format = "%Y-%m-%dT%H:%M:%S";
		strptime(payload.c_str(), format,& _time);

		//debug only
		Serial.print("Chamber time(UTC): ");
		char chDate[11] = "";
		char chTime[9] = "";
		strftime(chDate, 11, "%m/%d/%Y", &_time);
		strftime(chTime, 9, "%H:%M:%S", &_time);
		Serial.print(chDate);
		Serial.print(" ");
		Serial.println(chTime);

		int epoch_time = mktime(&_time);
		timeval epoch = { epoch_time, 0 };
		const timeval* tv = &epoch;
		settimeofday(tv, NULL);
		
		


	} else {
		Serial.println("Error on HTTP time request");
	}

	http.end();

	int rcode = setenv("TZ", "EST+5", 1);
	tzset();
	Serial.print("SetEnv reply");
	Serial.println(rcode);
	//VERIFICA
	struct tm now;
	getLocalTime(&now, 0);
	Serial.println(&now, " %B %d %Y %H:%M:%S (%A)");
	delay(1000);

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
	http.end();
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
	http.end();
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
	ss << (std::ceil(this->getChamber().getHumidity() * 10) / 10) << "%";
	ss << " Temp: ";
	ss << (std::ceil(this->getChamber().getTemperature() * 10) /10) << "°C";
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
}

void GrowlManager::setBME280Pin(int SCLPIN, int SDAPIN)
{
	_chamber.setBME280Pin( SCLPIN,  SDAPIN);
	_humiditySensor.setPid(SCLPIN);//uses BME PIN as id for hum
}

