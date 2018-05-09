#include "Electron_Peripheral.h"

#include "ParticleWiring.h"
#include "Parameter.h"
//#include <boost/variant.hpp>



Electron_Peripheral::Electron_Peripheral(std::shared_ptr<IConsoleOutput> console) :
  IPeripheralDevice(console)
{}

bool Electron_Peripheral::doSetup()
{
  	SensorValue humidity("battery", [&m_Fuel = m_Fuel]{ return m_Fuel.getSoC(); });
  	m_SensorValues.push_back(humidity);

  	m_Parameters.push_back(Parameter("deepsleeptimeseconds", 60*5)); //5mins
  	m_Parameters.push_back(Parameter("lightsleeptimeseconds", 5)); //5seconds
  	m_Parameters.push_back(Parameter("lowpowermode", 1)); //5seconds

  return true;
}
bool Electron_Peripheral::doMainLoop()
{
	return true;
}

void Electron_Peripheral::PrintInfo()
{
	m_Console->logMessage(String::format("Firmware Version: %s", System.version().c_str()));
	m_Console->logMessage(String::format("Free Memory: %u bytes", System.freeMemory()));
	m_Console->logMessage(String::format("Device ID: %s", System.deviceID().c_str()));
}