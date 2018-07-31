#include "IPeripheralDevice.h"

#include <algorithm>
//#include <boost/throw_exception.hpp>

/*
void boost::throw_exception(std::exception const & e)
{
	
}
*/
IPeripheralDevice::IPeripheralDevice(std::shared_ptr<IConsoleOutput> console) :
   m_Console(console),
   m_SetupDone(false)
{

};

bool IPeripheralDevice::Setup()
{
    if (!m_Console)
    {
        Serial.println("ERROR: m_Console DOESNT exist");
    }
    m_SetupDone = doSetup();
    return m_SetupDone;
}

bool IPeripheralDevice::MainLoop()
{
    return doMainLoop();
}

void IPeripheralDevice::PrintAllSensorValues()
{
	for (auto itr = m_SensorValues.begin(); itr != m_SensorValues.end(); ++itr)
	{
		m_Console->logMessage(String::format("%s %s %0.2f", GetName().c_str(), itr->GetName().c_str(), /*boost::get<double>*/(itr->GetCurrentValue())));
	}
}

void IPeripheralDevice::PrintParameters()
{
	for (auto itr = m_Parameters.begin(); itr != m_Parameters.end(); ++itr)
	{
		m_Console->logMessage(String::format("%s %s %0.2f - Hardcoded value of %0.2f", 
			GetName().c_str(), 
			itr->GetName().c_str(), 
			/*boost::get<double>*/(itr->GetCurrentValue()), 
			/*boost::get<double>*/(itr->GetHardCodedDefault())
			));
	}
}

void IPeripheralDevice::PrintFunctions()
{
	for (auto itr = m_Functions.begin(); itr != m_Functions.end(); ++itr)
	{
		m_Console->logMessage(itr->first.c_str());
	}
}




