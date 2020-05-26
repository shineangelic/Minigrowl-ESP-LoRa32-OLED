#include <GrowlDevice.h>

int GrowlDevice::getBoardId()
{
	return _boardId;
}

void GrowlDevice::setBoardId(int bid)
{
	_boardId = bid;
}

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

bool GrowlDevice::hasError()
{
	return _errorPresent;
}
