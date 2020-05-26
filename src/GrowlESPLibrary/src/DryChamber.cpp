#include <GrowlDevice.h>
#include <GrowlSensor.h>
#include <DryChamber.h>

#include <Ticker.h>

#include <sstream>
#include <ostream>
/*
https://github.com/beegee-tokyo/DHTesp/blob/master/examples/DHT_ESP32/DHT_ESP32.ino

uses beegee-tokyo DHT lib

represents a grow chamber, with two temperature&hum sensors and 4 commandable devices,
here named lights, outFan and inFan and Heater
*/

#define TIMER_INTERVAL_SECONDS 20
#define BME280_INTERVAL_MSEC 60000

/** Task handle to retrieve sensors value */
//void tempTask(void* pvParameters);
void triggerGetDryTemp();
void retrieveDTemperatureTask();
TaskHandle_t dryerTaskHandle = NULL;

TwoWire I2ConeDry = TwoWire(1);
Adafruit_BME280 Dbme;

float _dryerTemp;
float _dryerHum;
float _dryerPressure;
bool _bmeDryer280Err = true;

/** Ticker for temperature reading */
Ticker dryerTicker; 
/** Flag if task should run */
bool DtasksEnabled = false;


/* DEVONO ESSERE STATE GIA CHIAMATE LE SETPIN */
void DryChamber::init()
{
	Serial.println("GrowlChamber init");
	//DHT sensor
	bool initOK = initTemp();
	//BME280 sensor
	I2ConeDry.begin(_SDAPIN, _SCLPIN, BME280_INTERVAL_MSEC);
	bool status1 = Dbme.begin(0x76, &I2ConeDry);
	if (!status1) {
		Serial.println("Could not find a valid BME280_1 sensor, check wiring!");
		_tempSensor.setHasError(true);
		_humiditySensor.setHasError(true);
		initOK = false;
	}

	if (!initOK)
	{
		Serial.println("GrowlChamber init ERROR, please reboot");
		return;
	}
	// Signal end of setup() to tasks
	DtasksEnabled = true;
}

void DryChamber::loop()
{

	if (!DtasksEnabled) {
		// Wait 2 seconds to let system settle down
		delay(2000);
		init();
		// Enable task that will read values from the DHT sensor
		DtasksEnabled = true;
		if (dryerTaskHandle != NULL) {
			vTaskResume(dryerTaskHandle);
		}
	}
	//refresh sensors from static
	_barometer.setReading(_dryerPressure);
	//_lightSensor.setReading(getLumen());
	_tempSensor.setReading(_dryerTemp);
	_humiditySensor.setReading(_dryerHum);

	_barometer.setHasError(_bmeDryer280Err);
	_tempSensor.setHasError(_bmeDryer280Err);
	_humiditySensor.setHasError(_bmeDryer280Err); 



	//operate relays
	digitalWrite(_outTakeFan.getPid(), _outTakeFan.getReading());

	yield();
}

std::string DryChamber::reportStatus()
{
	struct tm timeinfo;
	getLocalTime(&timeinfo);

	char chTime[21] = "";
	strftime(chTime, 21, " %H:%M:%S %d/%m/%Y", &timeinfo);

	std::ostringstream ss;
	   
	ss << "FAN - OUT: ";
	ss << (this->getOuttakeFan()->getReading() != 0 ? "ON" : "OFF");
	ss << " PID: ";
	ss << (this->getOuttakeFan()->getPid());
	ss << "\n";
	ss << "Hum: ";
	ss << (std::ceil(this->getHumiditySensor()->getReading() * 10) / 10) << "%";
	ss << " Temp: ";
	ss << (std::ceil(this->getTemperatureSensor()->getReading() * 10) / 10) << "°C";
	ss << "\n";
	 
	ss << chTime;
	//std::string s(ss.str());
	return ss.str();
}

/**
 * initTemp
 * Setup DHT library
 * Setup task and timer for repeated measurement
 * @return bool
 *    true if task and timer are started
 *    false if task or timer couldn't be started
 */
bool DryChamber::initTemp() {
	byte resultValue = 0;
	// Initialize temperature sensor
	Serial.print("DryChamber::initTemp()");
 

	// Start task to get temperature
	xTaskCreatePinnedToCore(
		tempTask,                       /* Function to implement the task */
		"growlTask",                    /* Name of the task */
		4000,                           /* Stack size in words */
		NULL,                           /* Task input parameter */
		5,                              /* Priority of the task */
		&dryerTaskHandle,                /* Task handle. */
		1);                             /* Core where the task should run */

	if (dryerTaskHandle == NULL) {
		_humiditySensor.setHasError(true);
		_tempSensor.setHasError(true);
		Serial.println("Failed to start task for temperature update");
		return false;
	}
	else {
		// Start update of environment data every 20 seconds
		dryerTicker.attach(TIMER_INTERVAL_SECONDS, triggerGetDryTemp);
	}
	return true;
}


/**
 * triggerGetDryTemp
 * Sets flag dhtUpdated to true for handling in loop()
 * called by Ticker getTempTimer
 */
void  triggerGetDryTemp() {
	if (dryerTaskHandle != NULL) {
		xTaskResumeFromISR(dryerTaskHandle);
	}
}

/**
 * Task to reads temperature from DHT11 sensor
 * @param pvParameters
 *    pointer to task parameters
 */
void DryChamber::tempTask(void* pvParameters) {
	Serial.println("tempTask loop started");
	while (1) // tempTask loop
	{
		if (DtasksEnabled) {
			// Get temperature values
			retrieveDTemperatureTask();
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
void retrieveDTemperatureTask() {
	 

	float temperature = Dbme.readTemperature();
	float humidity = Dbme.readHumidity();
	float pressure = Dbme.readPressure() / 100.0F;
	Serial.print("BME280: T:");
	Serial.print(temperature);
	Serial.print(" H:");
	Serial.println(humidity);


	if (isnan(temperature) || isnan(humidity)) {
		Serial.println("Failed to read from BME280 sensor!");
		_bmeDryer280Err = true;
		return;
	}
	_bmeDryer280Err = false;
	_dryerTemp = temperature;
	_dryerHum = humidity;
	_dryerPressure = pressure;

}
 
 

bool DryChamber::switchOuttakeFan(bool on)
{
	_outTakeFan.setReading(on ? 1 : 0);
	digitalWrite(_outTakeFan.getPid(), on);
	return on;
}
 
 

void DryChamber::setMainLightsPin(int HWPIN)
{
	 
}

void DryChamber::setIntakeFanPin(int HWPIN)
{
	 
}

void DryChamber::setOuttakeFanPin(int HWPIN)
{
	_outTakeFan.setPid(HWPIN);

}

void DryChamber::setHeaterPin(int HWPIN)
{
	 
}

void DryChamber::setLightSensorPin(int HWPIN)
{
	 
}

void DryChamber::setDhtPin(int HWPIN)
{
	 
}

void DryChamber::setBME280Pin(int SCLPIN, int SDAPIN)
{
	_SCLPIN = SCLPIN;
	_SDAPIN = SDAPIN;
	_barometer.setPid(SDAPIN);
	_humiditySensor.setPid(SCLPIN);
	_tempSensor.setPid(SCLPIN + SDAPIN);
	Serial.println("dry chamber set BME PIN ");

}

bool DryChamber::hasErrors()
{
	return _bmeDryer280Err ;
}
 

OutTakeFan* DryChamber::getOuttakeFan()
{
	return &_outTakeFan;
}

 
float DryChamber::getPressure()
{
	return _dryerPressure;
}


BaromenterSensor* DryChamber::getBarometerSensor()
{
	return &_barometer;
}

TemperatureSensor* DryChamber::getTemperatureSensor()
{
	return &_tempSensor;
}

HumiditySensor* DryChamber::getHumiditySensor()
{
	return &_humiditySensor;
}
 
 


