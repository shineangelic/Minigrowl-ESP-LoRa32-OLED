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
#include <GrowlActuator.h>
#include <GrowlManager.h>
#include <GrowlCommand.h>

#define BOARD_ID 2
#define DRY_MODE 2

using namespace std;

const char* CONTENT_TYPE_JSON_UTF8 = "application/json;charset=UTF-8";

//const int hourSchedule[] = { 1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
const int BASE_SLEEP = 250;

const int MAX_SLEEP = 12000;//15 seconds
const int SLEEP_INCREMENT = 10;//100 cycles to increment a second

//cit. Menzi va veloce ogni tanto
int _sleepDelay = BASE_SLEEP;


//the pin has to be already set
void GrowlManager::initChamber(const char* serverhost, const int serverport, const char* prot)
{
	_host = serverhost;
	_httpPort = serverport;
	_proto = prot;
	_pc = 0;
	_chamber->init();

	if (_chamberMode != DRY_MODE) {
		GrowlChamber* cptr = (GrowlChamber*) &_chamber;
		_sensorsPtr.push_back(cptr->getLightSensor());
		_sensorsPtr.push_back(cptr->getTemperatureSensor());
		_sensorsPtr.push_back(cptr->getHumiditySensor());
		_sensorsPtr.push_back(cptr->getBarometerSensor());
		_sensorsPtr.push_back(cptr->getExternalTemperatureSensor());
		_sensorsPtr.push_back(cptr->getExternalHumiditySensor());

		_actuatorsPtr.push_back(cptr->getIntakeFan());
		_actuatorsPtr.push_back(cptr->getOuttakeFan());
		_actuatorsPtr.push_back(cptr->getMainLights());
		_actuatorsPtr.push_back(cptr->getHeater());
	}
	else {
		DryChamber* cptr = (DryChamber*)&_chamber;
		_actuatorsPtr.push_back(cptr->getIntakeFan());
		_actuatorsPtr.push_back(cptr->getOuttakeFan());
	}
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

	calcDelay();

	applyServerCommands();

	//let the chamber do its readings
	_chamber->loop();

	//these are sent one per PC
	if (_pc % 17 == 0 || _sburtoMode > 0) {
		sendRandomActuator();
		delay(200);
	}

	//send sensors a little faster
	if (_pc % 7 == 0 || _sburtoMode > 0) {
		sendRandomSensor();
		delay(200);
	}

	Serial.print("WILL SLEEP ");
	Serial.println(_sleepDelay);
	delay(_sleepDelay);
}

void GrowlManager::calcDelay() {
	if (!_commandsQueue.empty() || _sburtoMode > 0) {
		_sleepDelay = BASE_SLEEP;
	}

	if (_sleepDelay < MAX_SLEEP)
		_sleepDelay += SLEEP_INCREMENT;

	if (_sburtoMode > 0)
		_sburtoMode--;

}

void GrowlManager::chamberLogic()
{
	/*time_t tnow;
	time(&tnow);
	//int ret = localtime_s(&when, &tnow);
	struct tm* now = localtime(&tnow);
	char chTime[9] = ""; */

	/*
	struct tm timeinfo;
	//struct tm now;
	if (getLocalTime(&timeinfo)) {
		//LIFECYCLE
		Serial.print("LOCAL HOUR: ");
		Serial.println(timeinfo.tm_hour);
		
		//mainlights schedule
		if (_chamber.getMainLights()->getMode() == MODE_AUTO) {
			if (hourSchedule[timeinfo.tm_hour] != _chamber.getMainLights()->getReading()) {
				Serial.print("AUTO LIGHTS SWITCH: ");
				Serial.println(hourSchedule[timeinfo.tm_hour]);
				_chamber.switchMainLights(hourSchedule[timeinfo.tm_hour]);
			}
		}
		else
			Serial.println("MANUAL LIGHTS");
	}
	else
		Serial.println("getLocalTime() ERROR");

	//heat out take
	if (_chamber.getOuttakeFan()->getMode() == MODE_AUTO) {
		if (_chamber.getTemperatureSensor()->getReading() > 29) {
			_chamber.switchOuttakeFan(true);
		}
		else if (_chamber.getTemperatureSensor()->getReading() < 27) {
			_chamber.switchOuttakeFan(false);
		}
	}*/
}


void GrowlManager::applyServerCommands()
{
	Serial.print("QUEUE SIZE: ");
	Serial.println(_commandsQueue.size());

	if (!_commandsQueue.empty()) {
		GrowlCommand exec = _commandsQueue.front();
		//check for meta commands
		if (exec.getValueParameter() == CMD_FULLREFRESH && exec.getTargetActuatorId() == -1) {
			//simple refresh, sburto mode
			Serial.print("SBURTOMODE=");
			_sburtoMode = 5;
			_commandsQueue.pop_front();
			bool removed = removeExecutedCommand(&exec);
		}
		else {

			//look for actuator
			for (auto const& actuatorToSend : _actuatorsPtr) {
				//Se trova, fa pop della coda e delete sul srv
				if (actuatorToSend->getPid() == exec.getTargetActuatorId()) {
					_commandsQueue.pop_front();
					int ret = actuatorToSend->executeCommand(exec);
					Serial.print("COMMAND EXEC, RETURN=");
					Serial.println(ret);
					bool removed = removeExecutedCommand(&exec);

					delay(250);
					//let srv breath
					try
					{
						String url = "/api/esp/v1/actuators/id/";
						url += actuatorToSend->getPid();
						String completeUrl = String("") + _proto + _host + ":" + _httpPort + url;
						sendActuator(completeUrl, actuatorToSend);
						delay(500);//let srv breath
					}
					catch (const std::exception& we)
					{
						Serial.print("ERRORE UPDATE SECCO");
						Serial.println(we.what());
					}
					return;
				}
			}
			Serial.println("UNEXECUTED?");
			Serial.print("getValueParameter=");
			Serial.print(exec.getValueParameter());
			Serial.print(" getTargetActuatorId=");
			Serial.print(exec.getTargetActuatorId());
		}
	}

}

bool GrowlManager::removeExecutedCommand(GrowlCommand* executed) {

	WiFiClient client;
	HTTPClient http;

	// We now create a URI for the request
	String url = "/api/esp/v1/commands/id/";
	url += executed->getQueueId();
	String completeUrl = String("") + _proto + _host + ":" + _httpPort + url;
	Serial.print("removeExecutedCommand() url: ");
	Serial.println(completeUrl);

	http.begin(completeUrl);
	http.addHeader("Content-Type", CONTENT_TYPE_JSON_UTF8); //Specify content-type header
	std::string s("");
	int httpResponseCode = http.POST(s.c_str()); //Send the actual POST request

	Serial.print("removeExecutedCommand() RESPONSE:");
	Serial.println(httpResponseCode);
	http.end();
	return (httpResponseCode == 200);
}

void GrowlManager::retrieveServerCommands()
{
	WiFiClient client;
	HTTPClient http;

	if (!client.connect(_host, _httpPort)) {
		Serial.println("retrieveServerCommands() connection failed: ");
		Serial.print("host: ");
		Serial.print(_host);
		Serial.print(" port: ");
		Serial.print(_httpPort);
		return;
	}

	// We now create a URI for the request
	String urlPath = "/api/esp/v1/commands/";
	String completeUrl = String("") + _proto + _host + ":" + _httpPort + urlPath;

	Serial.print("retrieveServerCommands: ");
	Serial.println(completeUrl);

	bool began = http.begin(completeUrl); //Specify destination for HTTP request
	http.addHeader("Content-Type", CONTENT_TYPE_JSON_UTF8); //Specify content-type header
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
	}
	else {
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
	String completeUrl = String("") + _proto + _host + ":" + _httpPort + url;

	sendSensor(completeUrl, toSend);
}

void GrowlManager::sendSensor(String completeUrl, GrowlSensor* toSend)
{
	WiFiClient client;
	HTTPClient http;

	if (!client.connect(_host, _httpPort)) {
		Serial.println("sendSensors() connection FAIL");
		return;
	}

	Serial.print("sendSensors(): ");
	Serial.println(completeUrl);

	Serial.print("HTTP REQ:");
	const std::string vray = toSend->toJSON();
	Serial.println(vray.c_str());

	http.begin(completeUrl); //Specify destination for HTTP request
	http.addHeader("Content-Type", CONTENT_TYPE_JSON_UTF8); //Specify content-type header
	int httpResponseCode = http.PUT(vray.c_str()); //Send the actual POST request

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
	String completeUrl = String("") + _proto + _host + ":" + _httpPort + url;
	sendActuator(completeUrl, toSend);
}

void GrowlManager::sendActuator(const String completeUrl, GrowlActuator* toSend)
{
	WiFiClient client;
	HTTPClient http;

	if (!client.connect(_host, _httpPort)) {
		Serial.println("sendActuators() connection FAIL");
		return;
	}

	Serial.print("Send actuator: ");
	Serial.println(completeUrl);

	const std::string vray = toSend->toJSON();
	Serial.print("HTTP REQ:");
	Serial.println(vray.c_str());
	http.begin(completeUrl); //Specify destination for HTTP request
	http.addHeader("Content-Type", CONTENT_TYPE_JSON_UTF8); //Specify content-type header
	int httpResponseCode = http.PUT(vray.c_str()); //Send the actual POST request

	Serial.print("HTTP RESPONSE:");
	Serial.println(httpResponseCode);
	http.end();

}

std::string GrowlManager::reportStatus()
{
	/*time_t tnow;
	time(&tnow);
	//int ret = localtime_s(&when, &tnow);
	struct tm* when = localtime(&tnow);
	*/

	
	return _chamber->reportStatus();
}


Chamber* GrowlManager::getChamber()
{
	return _chamber;
}

void GrowlManager::initMainLights(int HWPIN)
{
	GrowlChamber* cptr = (GrowlChamber*)&_chamber;
	cptr->setMainLightsPin(HWPIN);
}

void GrowlManager::initIntakeFan(int HWPIN)
{
	Serial.println("initP");
	_chamber->setIntakeFanPin(HWPIN);
	Serial.print("initIntakeFan on PIN: ");
	Serial.println(HWPIN);
}

void GrowlManager::initOuttakeFanPin(int HWPIN)
{
	//GrowlChamber* cptr = (GrowlChamber*)&_chamber;
	_chamber->setOuttakeFanPin(HWPIN);
}

bool GrowlManager::hasChamberError()
{
	return _chamber->hasErrors();
}

void GrowlManager::setHeaterPin(int HWPIN)
{
	GrowlChamber* cptr = (GrowlChamber*)&_chamber;
	cptr->setHeaterPin(HWPIN);
}

void GrowlManager::initLightSensor(int HWPIN)
{
	GrowlChamber* cptr = (GrowlChamber*)&_chamber;
	cptr->setLightSensorPin(HWPIN);
}

void GrowlManager::setDhtPin(int HWPIN)
{
	GrowlChamber* cptr = (GrowlChamber*)&_chamber;
	cptr->setDhtPin(HWPIN);
}

void GrowlManager::setBME280Pin(int SCLPIN, int SDAPIN)
{
	_chamber->setBME280Pin(SCLPIN, SDAPIN);
}

