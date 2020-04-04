#pragma once
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <DHT_U.h>
#include <DHT.h>

#include <GrowlSensor.h>



class GrowlChamber
{

public:
	GrowlChamber(uint8_t dht_pin, uint8_t dht_type) : _dht(dht_pin, dht_type) {};
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
	int getMainLightsPin( );
	int getIntakeFanPin( );
	int getOuttakeFanPin( );
	int getHeaterPin( );
	int getLightSensorPin( );
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
	DHT _dht;
};

