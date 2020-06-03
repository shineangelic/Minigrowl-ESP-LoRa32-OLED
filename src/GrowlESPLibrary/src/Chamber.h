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
#include <GrowlActuator.h>
class Chamber
{
public:
	Chamber() {};
	virtual  void init() {};
	virtual void loop()=0;
	//function used by OLED diplay
	virtual std::string reportStatus()=0;
	//used to light an 'error' LED
	virtual bool hasErrors()=0;
	virtual float getHumidity();
	virtual float getTemperature();
	//virtual function that will be implemented on a device base
	virtual void setBME280Pin(int SCLPIN, int SDAPIN)=0;
	virtual void setMainLightsPin(int HWPIN) = 0;
	virtual void setIntakeFanPin(int HWPIN) = 0;
	virtual void setOuttakeFanPin(int HWPIN) = 0;
	virtual void setHeaterPin(int HWPIN) = 0;
	virtual void setLightSensorPin(int HWPIN) = 0;
	virtual void setDhtPin(int HWPIN) = 0; 
};