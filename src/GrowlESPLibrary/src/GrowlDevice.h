#pragma once
/*Abstract device, could be a Sensor or an Actuator.

It is bound to a real PIN, and is possibly in error

*/
class GrowlDevice
{
public:
	GrowlDevice() {
		_boardId = -1;
		_gpioid = -1;
		_errorPresent = true;
		_reading = -1;
	};
	GrowlDevice(int boardId) {
		_boardId = boardId; _gpioid = -1;
		_errorPresent = true;
		_reading = -1;
	};
	int			getBoardId();
	void		setBoardId(int bid);
	int			getPid();
	void		setPid(int pid);
	void		setReading(float newVal);
	float		getReading();
	void		setHasError(float inErr);
	bool		hasError();
protected:
	int			_gpioid;
	int			_boardId;
	float		_reading;
	bool		_errorPresent;
};



