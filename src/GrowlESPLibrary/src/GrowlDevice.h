#pragma once
class GrowlDevice
{
public:
	int getPid();
	void setPid(int pid);
	void setReading(float newVal);
	float getReading();
	void setHasError(float inErr);
protected:
	int			_gpioid;
	float		_reading;
	bool		_errorPresent;
};



