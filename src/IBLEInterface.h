#include <string>
#include "Adafruit_BluefruitLE_SPI.h"

class IBLEInterface
{
public:
	virtual void UART_OutputString(const std::string& outputString) = 0;
	virtual Adafruit_BluefruitLE_SPI* GetRawBLEPtr()  = 0;
	virtual bool IsConnectedToMaster() = 0;

};