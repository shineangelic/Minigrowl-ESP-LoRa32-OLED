#include <GrowlDevice.h>
#include <GrowlSensor.h>
#include <GrowlChamber.h>
#include <DHTesp.h>
#include <Ticker.h>
/*
https://github.com/beegee-tokyo/DHTesp/blob/master/examples/DHT_ESP32/DHT_ESP32.ino

uses beegee-tokyo DHT lib

represents a grow chamber, with two temperature&hum sensors and 4 commandable devices,
here named lights, outFan and inFan and Heater
*/

#define TIMER_INTERVAL_SECONDS 20
#define BME280_INTERVAL_MSEC 60000
#define EXT_OFFSET 32

/** Task handle to retrieve sensors value */
//void tempTask(void* pvParameters);
void triggerGetTemp();
void retrieveTemperatureTask();
TaskHandle_t tempTaskHandle = NULL;

DHTesp _dht;
TwoWire I2Cone = TwoWire(1);
Adafruit_BME280 bme;

float _curTemp;
float _curHum;
float _curTempExt;
float _curHumExt;
float _pressure;
bool _dhtErr, _bme280Err;

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
		_tempSensor.setHasError(true);
		_humiditySensor.setHasError(true);
		initOK = false;
	}

	if (!initOK)
	{
		Serial.println("GrowlChamber init ERROR, please reboot");
		_tempSensorExt.setHasError(true);
		_humiditySensorExt.setHasError(true);
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
	//refresh sensors from static
	_barometer.setReading(_pressure);
	_lightSensor.setReading(getLumen());
	_tempSensor.setReading(_curTemp);
	_humiditySensor.setReading(_curHum);
	_tempSensorExt.setReading(_curTempExt);
	_humiditySensorExt.setReading(_curHumExt);

	_barometer.setHasError(_bme280Err);
	_tempSensor.setHasError(_bme280Err);
	_humiditySensor.setHasError(_bme280Err);
	_tempSensorExt.setHasError(_dhtErr);
	_humiditySensorExt.setHasError(_dhtErr);

	//lampada accesa col buio?
	if (_lightSensor.getReading() < 200 && _mainLights.getReading() > 0) {
		_mainLights.setHasError(true);
	}
	else { _mainLights.setHasError(false); }

	//operate relays
	digitalWrite(_mainLights.getPid(), _mainLights.getReading());
	digitalWrite(_inTakeFan.getPid(), _inTakeFan.getReading());
	digitalWrite(_outTakeFan.getPid(), _outTakeFan.getReading());
	digitalWrite(_hvac.getPid(), _hvac.getReading());



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
		"growlTask",                    /* Name of the task */
		4000,                           /* Stack size in words */
		NULL,                           /* Task input parameter */
		5,                              /* Priority of the task */
		&tempTaskHandle,                /* Task handle. */
		1);                             /* Core where the task should run */

	if (tempTaskHandle == NULL) {
		_humiditySensor.setHasError(true);
		_tempSensor.setHasError(true);
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
void GrowlChamber::tempTask(void* pvParameters) {
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
		_dhtErr = true;
		//_tempSensorExt.setHasError(true);
		//return;
	}else {
		_dhtErr = false;
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
		_curTempExt = newValues.temperature;
		_curHumExt = newValues.humidity;
		Serial.println("DHT22:  T:" + String(newValues.temperature) + " H:" + String(newValues.humidity) + " I:" + String(heatIndex) + " D:" + String(dewPoint) + " " + comfortStatus);
	}

	float temperature = bme.readTemperature();
	float humidity = bme.readHumidity();
	float pressure = bme.readPressure() / 100.0F;
	Serial.print("BME280: T:");
	Serial.print(temperature);
	Serial.print(" H:");
	Serial.println(humidity);


	if (isnan(temperature) || isnan(humidity)) {
		Serial.println("Failed to read from BME280 sensor!");
		_bme280Err = true;
		return;
	}
	_bme280Err = false;
	_curTemp = temperature;
	_curHum = humidity;
	_pressure = pressure;

}



bool GrowlChamber::getMainLightsStatus()
{
	return _mainLights.getReading() > 0;
}

bool GrowlChamber::getIntakeFanStatus()
{
	return _inTakeFan.getReading() > 0;
}


bool GrowlChamber::getHeatingStatus()
{
	return _hvac.getReading() > 0;
}

bool GrowlChamber::switchMainLights(int on)
{
	_mainLights.setReading(on ? 1 : 0);
	digitalWrite(_mainLights.getPid(), _mainLights.getReading());
	return _mainLights.getReading();
}

bool GrowlChamber::switchIntakeFan(bool on)
{
	_inTakeFan.setReading(on ? 1 : 0);
	digitalWrite(_inTakeFan.getPid(), _inTakeFan.getReading());
	return _inTakeFan.getReading();
}

bool GrowlChamber::switchOuttakeFan(bool on)
{
	_outTakeFan.setReading(on ? 1 : 0);
	digitalWrite(_outTakeFan.getPid(), on);
	return on;
}

bool GrowlChamber::switchHeater(bool on)
{
	_hvac.setReading(on ? 1 : 0);
	digitalWrite(_hvac.getPid(), _hvac.getReading());
	return _hvac.getReading();
}

void GrowlChamber::setMainLightsPin(int HWPIN)
{
	_mainLights.setPid(HWPIN);
}

void GrowlChamber::setIntakeFanPin(int HWPIN)
{
	_inTakeFan.setPid(HWPIN);
}

void GrowlChamber::setOuttakeFanPin(int HWPIN)
{
	_outTakeFan.setPid(HWPIN);
}

void GrowlChamber::setHeaterPin(int HWPIN)
{
	_hvac.setPid(HWPIN);
}

void GrowlChamber::setLightSensorPin(int HWPIN)
{
	_lightSensor.setPid(HWPIN);
}

void GrowlChamber::setDhtPin(int HWPIN)
{
	_dht_pin = HWPIN;
	_tempSensor.setPid(HWPIN);
	_tempSensorExt.setPid(HWPIN + EXT_OFFSET);
}

void GrowlChamber::setBME280Pin(int SCLPIN, int SDAPIN)
{
	_SCLPIN = SCLPIN;
	_SDAPIN = SDAPIN;
	_barometer.setPid(SDA);
	_humiditySensor.setPid(SCLPIN);
	_humiditySensorExt.setPid(SCLPIN + EXT_OFFSET);

}

bool GrowlChamber::hasErrors()
{
	return _dhtErr || _bme280Err;
}

MainLights* GrowlChamber::getMainLights()
{
	return &_mainLights;
}

IntakeFan* GrowlChamber::getIntakeFan()
{
	return &_inTakeFan;
}

OutTakeFan* GrowlChamber::getOuttakeFan()
{
	return &_outTakeFan;
}

Hvac* GrowlChamber::getHeater()
{
	return &_hvac;
}

/*float GrowlChamber::getTemperature()
{
	return _curTemp;
}

float GrowlChamber::getHumidity()
{
	//object for static? Im no good at C++ :(
	return _curHum;
}
*/
float GrowlChamber::getPressure()
{
	return _pressure;
}


int GrowlChamber::getLumen()
{
	return analogRead(_lightSensor.getPid());
	//return (float)(1023 - sensorValue) * 10 / sensorValue;
	/*Serial.println("the analog read data is ");
	Serial.println(sensorValue);
	Serial.println("the sensor resistance is ");
	Serial.println(Rsensor, DEC);
	return 0.0f;*/
}

BaromenterSensor* GrowlChamber::getBarometerSensor()
{
	return &_barometer;
}

TemperatureSensor* GrowlChamber::getTemperatureSensor()
{
	return &_tempSensor;
}

HumiditySensor* GrowlChamber::getHumiditySensor()
{
	return &_humiditySensor;
}

TemperatureSensor* GrowlChamber::getExternalTemperatureSensor()
{
	return &_tempSensorExt;
}

HumiditySensor* GrowlChamber::getExternalHumiditySensor()
{
	return &_humiditySensorExt;
}

LightSensor* GrowlChamber::getLightSensor()
{
	return &_lightSensor;
}


