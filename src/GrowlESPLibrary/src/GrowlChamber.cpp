#include "GrowlChamber.h"
#include <DHTesp.h>
#include <Ticker.h>
/*
https://github.com/beegee-tokyo/DHTesp/blob/master/examples/DHT_ESP32/DHT_ESP32.ino

uses beegee-tokyo DHT lib
*/

void tempTask(void* pvParameters);
void triggerGetTemp();
float getTemperatureTask();


/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = NULL;
DHTesp _dht;

float _curTemp;
float _curHum;

/** Ticker for temperature reading */
Ticker tempTicker;
/** Comfort profile */
ComfortState cf;
/** Flag if task should run */
bool tasksEnabled = false;
 
void GrowlChamber::init()
{
	Serial.println("GrowlChamber init");
	initTemp();
	// Signal end of setup() to tasks
	tasksEnabled = true;
}

void GrowlChamber::loop()
{

	if (!tasksEnabled) {
		// Wait 2 seconds to let system settle down
		delay(2000);
		// Enable task that will read values from the DHT sensor
		tasksEnabled = true;
		if (tempTaskHandle != NULL) {
			vTaskResume(tempTaskHandle);
		}
	}
	yield();
	digitalWrite(_mainLightPIN, _isMainLightsON);
	digitalWrite(_intakePIN, _isIntakeFanON);
	digitalWrite(_outtakePIN, _isOuttakeON);
	digitalWrite(_heaterPIN, _isHeaterON);

}

/**
 * initTemp
 * Setup DHT library
 * Setup task and timer for repeated measurement
 * @return bool
 *    true if task and timer are started
 *    false if task or timer couldn't be started
 */
bool GrowlChamber::initTemp() {
	byte resultValue = 0;
	// Initialize temperature sensor
	_dht.setup(_dht_pin, DHTesp::DHT22);
	Serial.print("DHT initiated on PIN ");
	Serial.println(_dht_pin);

	// Start task to get temperature
	xTaskCreatePinnedToCore(
		tempTask,                       /* Function to implement the task */
		"tempTask ",                    /* Name of the task */
		4000,                           /* Stack size in words */
		NULL,                           /* Task input parameter */
		5,                              /* Priority of the task */
		&tempTaskHandle,                /* Task handle. */
		1);                             /* Core where the task should run */

	if (tempTaskHandle == NULL) {
		Serial.println("Failed to start task for temperature update");
		return false;
	}
	else {
		// Start update of environment data every 20 seconds
		tempTicker.attach(20, triggerGetTemp);
	}
	return true;
}


/**
 * triggerGetTemp
 * Sets flag dhtUpdated to true for handling in loop()
 * called by Ticker getTempTimer
 */
void  triggerGetTemp() {
	if (tempTaskHandle != NULL) {
		xTaskResumeFromISR(tempTaskHandle);
	}
}

/**
 * Task to reads temperature from DHT11 sensor
 * @param pvParameters
 *    pointer to task parameters
 */
void tempTask(void* pvParameters) {
	Serial.println("tempTask loop started");
	while (1) // tempTask loop
	{
		if (tasksEnabled) {
			// Get temperature values
			getTemperatureTask();
		}
		// Got sleep again
		vTaskSuspend(NULL);
	}
}

/**
 * getTemperature
 * Reads temperature from DHT11 sensor
 * @return bool
 *    true if temperature could be aquired
 *    false if aquisition failed
*/
float getTemperatureTask() {
	// Reading temperature for humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
	TempAndHumidity newValues = _dht.getTempAndHumidity();
	// Check if any reads failed and exit early (to try again).
	if (_dht.getStatus() != 0) {
		Serial.println("DHT22 error status: " + String(_dht.getStatusString()));
		return false;
	}

	float heatIndex = _dht.computeHeatIndex(newValues.temperature, newValues.humidity);
	float dewPoint = _dht.computeDewPoint(newValues.temperature, newValues.humidity);
	float cr = _dht.getComfortRatio(cf, newValues.temperature, newValues.humidity);

	String comfortStatus;
	switch (cf) {
	case Comfort_OK:
		comfortStatus = "Comfort_OK";
		break;
	case Comfort_TooHot:
		comfortStatus = "Comfort_TooHot";
		break;
	case Comfort_TooCold:
		comfortStatus = "Comfort_TooCold";
		break;
	case Comfort_TooDry:
		comfortStatus = "Comfort_TooDry";
		break;
	case Comfort_TooHumid:
		comfortStatus = "Comfort_TooHumid";
		break;
	case Comfort_HotAndHumid:
		comfortStatus = "Comfort_HotAndHumid";
		break;
	case Comfort_HotAndDry:
		comfortStatus = "Comfort_HotAndDry";
		break;
	case Comfort_ColdAndHumid:
		comfortStatus = "Comfort_ColdAndHumid";
		break;
	case Comfort_ColdAndDry:
		comfortStatus = "Comfort_ColdAndDry";
		break;
	default:
		comfortStatus = "Unknown:";
		break;
	};
	_curTemp = newValues.temperature;
	_curHum = newValues.humidity;
	Serial.println(" T:" + String(newValues.temperature) + " H:" + String(newValues.humidity) + " I:" + String(heatIndex) + " D:" + String(dewPoint) + " " + comfortStatus);
	return newValues.temperature;
}



bool GrowlChamber::getMainLightsStatus()
{
	return _isMainLightsON;
}

bool GrowlChamber::getIntakeFanStatus()
{
	return _isIntakeFanON;
}

bool GrowlChamber::getOuttakeFanStatus()
{
	return _isOuttakeON;
}

bool GrowlChamber::getHeatingStatus()
{
	return _isHeaterON;
}

bool GrowlChamber::switchMainLights(bool on)
{
	_isMainLightsON = on;
	digitalWrite(_mainLightPIN, _isMainLightsON);
	return _isMainLightsON;
}

bool GrowlChamber::switchIntakeFan(bool on)
{
	_isIntakeFanON = on;
	digitalWrite(_intakePIN, _isIntakeFanON);
	return _isIntakeFanON;
}

bool GrowlChamber::switchOuttakeFan(bool on)
{
	_isOuttakeON = on;
	digitalWrite(_outtakePIN, _isOuttakeON);
	return _isOuttakeON;
}

bool GrowlChamber::switchHeater(bool on)
{
	_isHeaterON = on;
	digitalWrite(_heaterPIN, _isHeaterON);
	return _isHeaterON;
}

void GrowlChamber::setMainLightsPin(int HWPIN)
{
	_mainLightPIN = HWPIN;
}

void GrowlChamber::setIntakeFanPin(int HWPIN)
{
	_intakePIN = HWPIN;
}

void GrowlChamber::setOuttakeFanPin(int HWPIN)
{
	_outtakePIN = HWPIN;
}

void GrowlChamber::setHeaterPin(int HWPIN)
{
	_heaterPIN = HWPIN;
}

void GrowlChamber::setLightSensorPin(int HWPIN)
{
	_lightSensorPIN = HWPIN;
}

void GrowlChamber::setDhtPin(int HWPIN)
{
	_dht_pin = HWPIN;
}

int GrowlChamber::getMainLightsPin()
{
	return _mainLightPIN;
}

int GrowlChamber::getIntakeFanPin()
{
	return _intakePIN;
}

int GrowlChamber::getOuttakeFanPin()
{
	return _outtakePIN;
}

int GrowlChamber::getHeaterPin()
{
	return _heaterPIN;
}

int GrowlChamber::getLightSensorPin()
{
	return _lightSensorPIN;
}

float GrowlChamber::getTemperature()
{
	return _curTemp;
}

float GrowlChamber::getHumidity()
{
	/*float h = _dht.getHumidity();
	if (isnan(h)) {
		Serial.println(F("Failed to read from DHT sensor!"));
		return -1;
	}*/
	return _curHum;
}
/*
float GrowlChamber::getTemperature()
{

	float t = _dht.getTemperature();
	//float f = _dht.readTemperature(false, true);

	if (isnan(t)) {
		Serial.println(F("Failed to read from DHT sensor!"));
		return -1;
	}

	return t;
	//float hif = _dht.computeHeatIndex(f, h);
	//float hic = _dht.computeHeatIndex(t, h, false);
}
*/


int GrowlChamber::getLumen()
{
	return analogRead(_lightSensorPIN);
	//return (float)(1023 - sensorValue) * 10 / sensorValue;
	/*Serial.println("the analog read data is ");
	Serial.println(sensorValue);
	Serial.println("the sensor resistance is ");
	Serial.println(Rsensor, DEC);
	return 0.0f;*/
}

