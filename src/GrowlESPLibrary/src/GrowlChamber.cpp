#include "GrowlChamber.h"
#include <DHTesp.h>
#include <Ticker.h>
/*
https://github.com/beegee-tokyo/DHTesp/blob/master/examples/DHT_ESP32/DHT_ESP32.ino

uses beegee-tokyo DHT lib
*/
#define TIMER_INTERVAL_SECONDS 20
#define BME280_INTERVAL_MSEC 100000

/** Task handle to retrieve sensors value */
void tempTask(void* pvParameters);
void triggerGetTemp();
void retrieveTemperatureTask();
TaskHandle_t tempTaskHandle = NULL;

DHTesp _dht;
TwoWire I2Cone = TwoWire(1);
Adafruit_BME280 bme;

float _curTemp;
float _curHum;

/** Ticker for temperature reading */
Ticker tempTicker;
/** Comfort profile */
ComfortState cf;
/** Flag if task should run */
bool tasksEnabled = false;




/* DEVONO ESSERE STATE GIA CHIAMATE LE SETPIN */
void GrowlChamber::init()
{
	Serial.println("GrowlChamber init");
	//DHT sensor
	bool initOK = initTemp();
	//BME280 sensor
	I2Cone.begin(_SDAPIN, _SCLPIN, BME280_INTERVAL_MSEC);
	bool status1 = bme.begin(0x76, &I2Cone);
	if (!status1) {
		Serial.println("Could not find a valid BME280_1 sensor, check wiring!");
		initOK = false;
	}

	if (!initOK)
	{
		Serial.println("GrowlChamber init ERROR, please reboot");
		return;
	}

	// Signal end of setup() to tasks
	tasksEnabled = true;
}

void GrowlChamber::loop()
{

	if (!tasksEnabled) {
		// Wait 2 seconds to let system settle down
		delay(2000);
		init();
		// Enable task that will read values from the DHT sensor
		tasksEnabled = true;
		if (tempTaskHandle != NULL) {
			vTaskResume(tempTaskHandle);
		}
	}

	//operate relays
	digitalWrite(_mainLightPIN, _isMainLightsON);
	digitalWrite(_intakePIN, _isIntakeFanON);
	digitalWrite(_outtakePIN, _isOuttakeON);
	digitalWrite(_heaterPIN, _isHeaterON);



	yield();
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
	Serial.print("DHT init on PIN ");
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
		tempTicker.attach(TIMER_INTERVAL_SECONDS, triggerGetTemp);
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
			retrieveTemperatureTask();
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
void retrieveTemperatureTask() {
	// Reading temperature for humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
	TempAndHumidity newValues = _dht.getTempAndHumidity();
	// Check if any reads failed and exit early (to try again).
	if (_dht.getStatus() != 0) {
		Serial.println("DHT22 error status: " + String(_dht.getStatusString()));
		//return;
	}
	else {

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
		Serial.println("DHT22 update:" + String(newValues.temperature) + " H:" + String(newValues.humidity) + " I:" + String(heatIndex) + " D:" + String(dewPoint) + " " + comfortStatus);
	}
	float temperature = bme.readTemperature();
	float humidity = bme.readHumidity();
	//float pressure = bme.readPressure() / 100.0F;
	Serial.print("BME280: temperature: ");
	Serial.print(temperature);
	Serial.print(" BME280 humidity: ");
	Serial.println(humidity);


	//failsafe per DHT22 che fa schifo
	if (_curTemp != 0) {
		_curTemp = (_curTemp + temperature) / 2;
		_curHum = (_curHum + humidity) / 2;
	}
	else {
		_curTemp = temperature;
		_curHum = humidity;
	}
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

void GrowlChamber::setBME280Pin(int SCLPIN, int SDAPIN)
{
	_SCLPIN = SCLPIN;
	_SDAPIN = SDAPIN;
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
	//object for static? Im no good at C++ :(
	return _curHum;
}


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

