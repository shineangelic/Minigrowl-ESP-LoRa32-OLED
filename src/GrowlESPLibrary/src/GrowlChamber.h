#pragma once
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <DHTesp.h>
//Libraries for BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <GrowlSensor.h>




class GrowlChamber
{

public:
	GrowlChamber() {
		//_dht_pin = dht_pin;
		
		/*_dht = DHT(dht_pin, dht_type);
		Serial.print("GrowlChamber constructor, DHT pin=");
		Serial.println(dht_pin);
		_dht.begin();*/
	};
	void init();
	void loop();
	bool getMainLightsStatus();
	bool getIntakeFanStatus();
	bool getOuttakeFanStatus();
	bool getHeatingStatus();
	bool switchMainLights(bool on);
	bool switchIntakeFan(bool on);
	bool switchOuttakeFan(bool on);
	bool switchHeater(bool on);
	void setMainLightsPin(int HWPIN);
	void setIntakeFanPin(int HWPIN);
	void setOuttakeFanPin(int HWPIN);
	void setHeaterPin(int HWPIN);
	void setLightSensorPin(int HWPIN);
	void setDhtPin(int HWPIN);
	void setBME280Pin(int SCLPIN, int SDAPIN);
	int getMainLightsPin();
	int getIntakeFanPin();
	int getOuttakeFanPin();
	int getHeaterPin();
	int getLightSensorPin();
	float getTemperature();
	float getHumidity();
	int getLumen();//NOT real Lumen!
	
private:
	int _mainLightPIN;
	int _intakePIN;
	int _outtakePIN;
	int _heaterPIN;
	int _lightSensorPIN;
	
	bool _isMainLightsON;
	bool _isIntakeFanON;
	bool _isOuttakeON;
	bool _isHeaterON;

	bool initTemp();
	int _dht_pin;
	//int _dht_type;
	int _SCLPIN;
	int _SDAPIN;

};

