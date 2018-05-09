#pragma once
#include "IPeripheralDevice.h"

#include <DS18B20.h>
#include <map>

class DS18B20_Peripheral : public IPeripheralDevice
{
public:
    DS18B20_Peripheral(std::shared_ptr<IConsoleOutput> console, uint8_t ds18b20_pin, bool something);

    virtual std::string GetName() { return "DS18B20";  }
    virtual void PrintInfo();
    std::map<std::string, double> ReadSensorValues();

protected:
    virtual bool doSetup();
    virtual bool doMainLoop();

private:
	double getTheTemperature();
	String GetAddressAsString(std::array<uint8_t,8>& theAddress);


private:
	std::map<std::string, double> m_TemperatureC;
    DS18B20  m_DS18B20;

    std::vector<std::array<uint8_t,8>> m_Addresses;
};
