#include "ParticleWiring.h"
#include "Parameter.h"
#include "MCP9808_Peripheral.h"

//#include <boost/variant.hpp>

// This example assumes the sensor to be plugged into CONN2
//#define DHTPIN D1     // what pin we're connected to
// Here we define the type of sensor used
//#define DHTTYPE DHT11        // DHT 11



MCP9808_Peripheral::MCP9808_Peripheral(std::shared_ptr<IConsoleOutput> console, std::vector<uint8_t> pinsSetToInput, uint8_t alertPin) :
  IPeripheralDevice(console),
  m_pinsSetToInput(pinsSetToInput),
  m_alertPin(alertPin),
  m_DeviceFound(false)
{
  
}

bool MCP9808_Peripheral::doSetup()
{
  for (auto itr = m_pinsSetToInput.begin(); itr != m_pinsSetToInput.end(); ++itr)
  {
      pinMode(*itr, INPUT);
  }

  pinMode(m_alertPin, INPUT);  //ALERT PIN

  int waitForSeconds = 3;
  while(! m_MCP.begin())
  {
      m_Console->logMessage("MCP9808 not found");
      delay(1000);
      waitForSeconds--;
      if (waitForSeconds < 0) return false;
  }
  m_DeviceFound = true;

  SensorValue tempOnBoard("tempOnBoard", [&m_MCP = m_MCP]{ return m_MCP.getTemperature(); });
  m_SensorValues.push_back(tempOnBoard);
  return true;
}
bool MCP9808_Peripheral::doMainLoop()
{
	//SensorValue::Data_t something = m_Parameters.begin()->GetCurrentValue(); //humidity.GetCurrentValue();

	//m_Humidity_old= boost::get<double>(something);
	//m_TemperatureC = m_DHT.getTempCelcius();

	return true;
}

void MCP9808_Peripheral::PrintInfo()
{
  m_Console->logMessage(String::format("MCP9808 found: %s", m_DeviceFound ? "yes" : "no"));

}
