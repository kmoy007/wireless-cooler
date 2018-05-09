#include "DHT_Peripheral.h"

#include "ParticleWiring.h"
#include "Parameter.h"
//#include <boost/variant.hpp>

// This example assumes the sensor to be plugged into CONN2
//#define DHTPIN D1     // what pin we're connected to
// Here we define the type of sensor used
//#define DHTTYPE DHT11        // DHT 11



DHT_Peripheral::DHT_Peripheral(std::shared_ptr<IConsoleOutput> console, uint8_t dht_pin, uint8_t dht_type) :
  IPeripheralDevice(console),
  m_DHT(dht_pin, dht_type)
{}

bool DHT_Peripheral::doSetup()
{
  m_DHT.begin();

  SensorValue humidity("humidity", [&m_DHT = m_DHT]{ return m_DHT.getHumidity(); });
  m_SensorValues.push_back(humidity);

  SensorValue temperatureC("temperatureC", [&m_DHT = m_DHT]{ return m_DHT.getTempCelcius(); });
  m_SensorValues.push_back(temperatureC);
  return true;
}
bool DHT_Peripheral::doMainLoop()
{
	//SensorValue::Data_t something = m_Parameters.begin()->GetCurrentValue(); //humidity.GetCurrentValue();

	//m_Humidity_old= boost::get<double>(something);
	//m_TemperatureC = m_DHT.getTempCelcius();

	return true;
}

void DHT_Peripheral::PrintInfo()
{
  m_Console->logMessage("Nothing to Print for DHT devices");
}
