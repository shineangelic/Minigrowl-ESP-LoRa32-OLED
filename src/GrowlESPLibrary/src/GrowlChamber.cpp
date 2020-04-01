#include "GrowlChamber.h"
#include <DHT.h>


DHT _dht(DHTPIN, DHTTYPE);
void GrowlChamber::init()
{
	
	_dht.begin();

}

void GrowlChamber::loop()
{
	//trigger REAL READ
	float f = _dht.readTemperature(false, true);
}

bool GrowlChamber::getMainLightsStatus()
{
	return _isMainLightsON;
}

bool GrowlChamber::switchMainLights(bool on)
{
	_isMainLightsON = on;
	digitalWrite(_mainLightPIN, on);

	return _isMainLightsON;
}

void GrowlChamber::setMainLightsPin(int HWPIN)
{
	_mainLightPIN = HWPIN;
}

float GrowlChamber::getTemperature()
{
	
	float t = _dht.readTemperature(false, false);
	//float f = _dht.readTemperature(false, true);

	if (isnan(t)) {
		Serial.println(F("Failed to read from DHT sensor!"));
		return -1;
	}

	return t;
	//float hif = _dht.computeHeatIndex(f, h);
	//float hic = _dht.computeHeatIndex(t, h, false);

}

float GrowlChamber::getHumidity()
{
	float h = _dht.readHumidity();
	if (isnan(h)) {
		Serial.println(F("Failed to read from DHT sensor!"));
		return -1;
	}
	return h;
}

float GrowlChamber::getLumen()
{
	int sensorValue = analogRead(LIGHT_SENSOR);
	return (float)(1023 - sensorValue) * 10 / sensorValue;
	/*Serial.println("the analog read data is ");
	Serial.println(sensorValue);
	Serial.println("the sensor resistance is ");
	Serial.println(Rsensor, DEC);
	return 0.0f;*/
}

GrowlChamber::GrowlChamber()
{ 
}
