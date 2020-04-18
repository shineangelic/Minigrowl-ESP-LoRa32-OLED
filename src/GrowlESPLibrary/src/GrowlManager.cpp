﻿
#include <WiFi.h>
#include <HTTPClient.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sstream>
#include <ostream>

#define ARDUINOJSON_ENABLE_STD_STRING 1
#include <ArduinoJson.h>
#include <GrowlDevice.h>
#include <GrowlSensor.h>
#include <GrowlManager.h>
#include <GrowlCommand.h>


using namespace std;

const char* CONTENT_TYPE_JSON = "application/json;charset=UTF-8";
const char* host = "192.168.0.54";
const int httpPort = 8080;
const float hourSchedule[] = { 1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

const int MAX_SLEEP = 15000;//15 seconds
const int SLEEP_INCREMENT = 10;//100 cycles to increment a second

//cit. Menzi va veloce ogni tanto
int _sleepDelay = 500;


//the pin has to be already set
void GrowlManager::initChamber()
{
	_pc = 0;
	_chamber.init();

	_sensorsPtr.push_back(_chamber.getLightSensor());
	_sensorsPtr.push_back(_chamber.getTemperatureSensor());
	_sensorsPtr.push_back(_chamber.getHumiditySensor());
	_sensorsPtr.push_back(_chamber.getBarometerSensor());
	_sensorsPtr.push_back(_chamber.getExternalTemperatureSensor());
	_sensorsPtr.push_back(_chamber.getExternalHumiditySensor());

	_actuatorsPtr.push_back(_chamber.getIntakeFan());
	_actuatorsPtr.push_back(_chamber.getOuttakeFan());
	_actuatorsPtr.push_back(_chamber.getMainLights());
	_actuatorsPtr.push_back(_chamber.getHeater());
}

void GrowlManager::loop()
{
	if (_pc == 0)//jump zero
		_pc++;

	_pc++;

	//schedules and conditions
	chamberLogic();
	//retrieves new commands
	retrieveServerCommands();

	applyServerCommands();

	//let the chamber do its readings
	_chamber.loop();

	sendRandomSensor();
	delay(2000);
	sendRandomActuator();
	delay(2000);

	//these are sent one per PC
	/*if (_pc % 17 == 0) {
		delay(500);
		sendRandomActuator();
	}

	//send sensors a little faster
	if (_pc % 13 == 0) {
		delay(500);
		sendRandomSensor();
	}

	Serial.print("WILL SLEEP ");
	Serial.println(_sleepDelay);
	delay(_sleepDelay);*/
	//non va, per la storia UTC. Alla fine uso NTP
	//retrieveTime();
}

void GrowlManager::chamberLogic()
{
	//_chamber.switchHeater(true);
	//_chamber.switchMainLights(true);

	//LIFECYCLE
	struct tm now;
	getLocalTime(&now, 0);

	//mainlights schedule
	if (_chamber.getMainLights()->getMode() == MODE_AUTO)
		_chamber.switchMainLights(hourSchedule[now.tm_hour]);
	//_mainLights.setReading(hourSchedule[now.tm_hour]);
	//heat out take
	if (_chamber.getTemperatureSensor()->getReading() > 29) {
		_chamber.switchOuttakeFan(true);
	}
	else {
		_chamber.switchOuttakeFan(false);
	}
}


void GrowlManager::applyServerCommands()
{
	Serial.print("QUEUE SIZE: ");
	Serial.println(_commandsQueue.size());

	if (!_commandsQueue.empty()) {
		GrowlCommand exec = _commandsQueue.front();
		//look for actuator
		for (auto const& actuatorToSend : _actuatorsPtr) {
			//Se trova, fa pop della coda e delete sul srv
			if (actuatorToSend->getPid() == exec.getTargetActuatorId()) {
				_commandsQueue.pop_front();
				int ret = actuatorToSend->executeCommand(exec);
				Serial.print("COMMAND EXEC, RETURN=");
				Serial.println(ret);
				bool removed = removeExecutedCommand(&exec);
				delay(500);//let srv breath
				try
				{
					String url = "/api/esp/v1/actuators/id/";
					url += actuatorToSend->getPid();
					String completeUrl = String("http://") + host + ":" + httpPort + url;
					sendActuator(completeUrl, actuatorToSend);
					delay(500);//let srv breath
				}
				catch (const std::exception& we)
				{
					Serial.print("ERRORE UPDATE SECCO");
					Serial.println(we.what());
				}

				break;
			}

		}
	}

}

bool GrowlManager::removeExecutedCommand(GrowlCommand* executed) {

	WiFiClient client;
	HTTPClient http;

	// We now create a URI for the request
	String url = "/api/esp/v1/commands/id/";
	url += executed->getQueueId();
	String completeUrl = String("http://") + host + ":" + httpPort + url;
	Serial.print("removeExecutedCommand() url: ");
	Serial.println(completeUrl);

	http.begin(completeUrl);
	http.addHeader("Content-Type", CONTENT_TYPE_JSON); //Specify content-type header
	std::string s("");
	int httpResponseCode = http.POST(s.c_str()); //Send the actual POST request

	Serial.print("removeExecutedCommand() RESPONSE:");
	Serial.println(httpResponseCode);
	//Serial.println(_lightSensor.toJSON().c_str());
	http.end();
	return (httpResponseCode == 200);
}

void GrowlManager::retrieveServerCommands()
{
	WiFiClient client;
	HTTPClient http;

	if (!client.connect(host, httpPort)) {
		Serial.println("retrieveServerCommands() connection failed");
		return;
	}

	// We now create a URI for the request
	String urlPath = "/api/esp/v1/commands/";
	String completeUrl = String("http://") + host + ":" + httpPort + urlPath;

	Serial.print("retrieveServerCommands: ");
	Serial.println(completeUrl);

	http.begin(completeUrl); //Specify destination for HTTP request
	http.addHeader("Content-Type", CONTENT_TYPE_JSON); //Specify content-type header
	int httpResponseCode = http.GET();

	if (httpResponseCode == 200) { //Check for the returning code

		String payload = http.getString();
		Serial.println(payload);

		// allocate the memory for the document
		DynamicJsonDocument doc(payload.length() + 100);

		// parse a JSON array
		DeserializationError err = deserializeJson(doc, payload);
		if (err) {
			Serial.print(F("deserializeJson() failed with code "));
			Serial.println(err.c_str());
			http.end();
			return;
		}

		// extract the values
		JsonArray array = doc.as<JsonArray>();
		for (JsonVariant v : array) {
			GrowlCommand newC = GrowlCommand(v);
			bool skip = false;
			//se c'e` gia` scartalo
			for (GrowlCommand& cmdCheck : _commandsQueue) {
				if (cmdCheck.getQueueId() == newC.getQueueId()) {
					Serial.println("Skippin Queue push");
					skip = true;
					break;
				}
			}
			if (!skip) {
				Serial.print("Queue push ID: ");
				Serial.println(newC.getQueueId());
				_commandsQueue.push_front(newC);
			}
		}
	}else {
		Serial.print("Error on HTTP request: ");
		Serial.println(httpResponseCode);
	}
	http.end();
}

void GrowlManager::sendRandomSensor()
{
	GrowlSensor* toSend = _sensorsPtr.at(_pc % _sensorsPtr.size());
	// We now create a URI for the request
	String url = "/api/esp/v1/sensors/id/";
	url += toSend->getPid();
	String completeUrl = String("http://") + host + ":" + httpPort + url;

	sendSensor(completeUrl, toSend);
}

void GrowlManager::sendSensor(String completeUrl, GrowlSensor* toSend)
{
	WiFiClient client;
	HTTPClient http;

	if (!client.connect(host, httpPort)) {
		Serial.println("sendSensors() connection FAIL");
		return;
	}

	Serial.print("sendSensors(): ");
	Serial.println(completeUrl);

	Serial.print("HTTP REQ:");
	Serial.println(toSend->toJSON().c_str());

	http.begin(completeUrl); //Specify destination for HTTP request
	http.addHeader("Content-Type", "application/json;charset=UTF-8"); //Specify content-type header
	int httpResponseCode = http.PUT(toSend->toJSON().c_str()); //Send the actual POST request

	Serial.print("sendSensors() HTTP RESPONSE:");
	Serial.println(httpResponseCode);
	//Serial.println(_lightSensor.toJSON().c_str());
	http.end();
}


void GrowlManager::sendRandomActuator()
{
	GrowlActuator* toSend = _actuatorsPtr.at(_pc % _actuatorsPtr.size());
	String url = "/api/esp/v1/actuators/id/";
	url += toSend->getPid();
	String completeUrl = String("http://") + host + ":" + httpPort + url;
	sendActuator(completeUrl, toSend);
}

void GrowlManager::sendActuator(const String completeUrl, GrowlActuator* toSend)
{
	WiFiClient client;
	HTTPClient http;

	if (!client.connect(host, httpPort)) {
		Serial.println("sendActuators() connection FAIL");
		return;
	}

	Serial.print("Send actuator: ");
	Serial.println(completeUrl);
	
	const std::string vray = toSend->toJSON();
	Serial.print("HTTP REQ:");
	Serial.println(vray.c_str());
	http.begin(completeUrl); //Specify destination for HTTP request
	http.addHeader("Content-Type", CONTENT_TYPE_JSON); //Specify content-type header
	int httpResponseCode = http.PUT(vray.c_str()); //Send the actual POST request

	Serial.print("HTTP RESPONSE:");
	Serial.println(httpResponseCode);
	http.end();

}



std::string GrowlManager::reportStatus()
{
	time_t tnow;
	time(&tnow);
	//int ret = localtime_s(&when, &tnow);
	struct tm* when = localtime(&tnow);
	char chTime[9] = "";
	strftime(chTime, 9, "%H:%M:%S", when);

	std::ostringstream ss;
	ss << "LIGHT: ";
	ss << (_chamber.getMainLights()->getReading() != 0 ? "ON" : "OFF");
	ss << " HEATER: ";
	ss << (_chamber.getHeater()->getReading() != 0 ? "ON" : "OFF");
	ss << "\n";
	ss << "FAN: IN: ";
	ss << (_chamber.getIntakeFan()->getReading() != 0 ? "ON" : "OFF");
	ss << " - OUT: ";
	ss << (_chamber.getOuttakeFan()->getReading() != 0 ? "ON" : "OFF");
	ss << "\n";
	ss << "Hum: ";
	ss << (std::ceil(this->getChamber().getHumiditySensor()->getReading() * 10) / 10) << "%";
	ss << " Temp: ";
	ss << (std::ceil(this->getChamber().getTemperatureSensor()->getReading() * 10) / 10) << "°C";
	ss << "\n";
	ss << this->getChamber().getLumen() << "LUX   ";
	ss << " T.ext: ";
	ss << (std::ceil(this->getChamber().getExternalTemperatureSensor()->getReading() * 10) / 10) << "°C";
	ss << "\n";
	ss << chTime;
	//std::string s(ss.str());
	return ss.str();
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
}

void GrowlManager::initIntakeFan(int HWPIN)
{
	_chamber.setIntakeFanPin(HWPIN);
}

void GrowlManager::initOuttakeFanPin(int HWPIN)
{
	_chamber.setOuttakeFanPin(HWPIN);
}

bool GrowlManager::hasChamberError()
{
	return _chamber.hasErrors();
}

void GrowlManager::setHeaterPin(int HWPIN)
{
	_chamber.setHeaterPin(HWPIN);
}

void GrowlManager::initLightSensor(int HWPIN)
{
	_chamber.setLightSensorPin(HWPIN);
}

void GrowlManager::setDhtPin(int HWPIN)
{
	_chamber.setDhtPin(HWPIN);
}

void GrowlManager::setBME280Pin(int SCLPIN, int SDAPIN)
{
	_chamber.setBME280Pin(SCLPIN, SDAPIN);
}

