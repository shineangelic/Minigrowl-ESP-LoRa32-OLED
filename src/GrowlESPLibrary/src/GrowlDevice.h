#pragma once
/*Abstract device, could be a Sensor or an Actuator*/
class GrowlDevice
{
public:
	int			getPid();
	void		setPid(int pid);
	void		setReading(float newVal);
	float		getReading();
	void		setHasError(float inErr);
	bool		hasError();
protected:
	int			_gpioid;
	float		_reading;
	bool		_errorPresent;
};



