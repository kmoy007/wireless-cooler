#include "PwrMgmt_Peripheral.h"

#include "ParticleWiring.h"
#include "Parameter.h"
//#include <boost/variant.hpp>

// This example assumes the sensor to be plugged into CONN2
//#define DHTPIN D1     // what pin we're connected to
// Here we define the type of sensor used
//#define DHTTYPE DHT11        // DHT 11



PwrMgmt_Peripheral::PwrMgmt_Peripheral(std::shared_ptr<IConsoleOutput> console, uint8_t power_pin) :
  IPeripheralDevice(console),
  m_pin(power_pin)
{
  doSetup();
}

PwrMgmt_Peripheral::~PwrMgmt_Peripheral()
{
  pinMode(m_pin, OUTPUT);
  digitalWrite(m_pin, LOW); //turn that shit off
 // digitalWrite(ParticleWiring::pin_D7, LOW); //turn that shit off
}

bool PwrMgmt_Peripheral::doSetup()
{
  pinMode(m_pin, OUTPUT);
  digitalWrite(m_pin, HIGH);
//  digitalWrite(ParticleWiring::pin_D7, HIGH); //turn that shit off
  delay(1000);// allow devices to start
  return true;
}
bool PwrMgmt_Peripheral::doMainLoop()
{
	//SensorValue::Data_t something = m_Parameters.begin()->GetCurrentValue(); //humidity.GetCurrentValue();

	//m_Humidity_old= boost::get<double>(something);
	//m_TemperatureC = m_DHT.getTempCelcius();

	return true;
}

void PwrMgmt_Peripheral::PrintInfo()
{
  m_Console->logMessage("Nothing to Print for PwrMgmt device");
}
