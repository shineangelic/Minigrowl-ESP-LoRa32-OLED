#include "GrowlDevice.h"

int GrowlDevice::getPid()
{
	return _gpioid;
}

void GrowlDevice::setPid(int pid)
{
	_gpioid = pid;
}
void GrowlDevice::setReading(float newVal)
{
	_reading = newVal;
}
float GrowlDevice::getReading()
{
	return _reading;
}

void GrowlDevice::setHasError(float inErr)
{
	_errorPresent = inErr;
}
