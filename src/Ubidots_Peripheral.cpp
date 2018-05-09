#include "Ubidots_Peripheral.h"

#include "ParticleWiring.h"
#include "Parameter.h"
//#include <boost/variant.hpp>

//#include <Ubidots.h>

// This example assumes the sensor to be plugged into CONN2
//#define DHTPIN D1     // what pin we're connected to
// Here we define the type of sensor used
//#define DHTTYPE DHT11        // DHT 11



Ubidots_Peripheral::Ubidots_Peripheral(std::shared_ptr<IConsoleOutput> console, 
      const std::string& ubidots_token, 
      const std::string& ubidots_device_name, 
      const std::string& ubidots_device_label) 
:
  IPeripheralDevice(console),
  m_ubidots_device_label(ubidots_device_label),
  m_ubidots_device_name(ubidots_device_name)
{
  m_Ubidots = std::unique_ptr<Ubidots>(new Ubidots(GetCharPtrOfStdString_WatchMemory(ubidots_token)));
  doSetup();  // need to make sure we are always first so other things are turned on.
}

Ubidots_Peripheral::~Ubidots_Peripheral()
{
  
}
char * Ubidots_Peripheral::GetCharPtrOfStdString_WatchMemory(const std::string& theString)
{
   char * writable = new char[theString.size() + 1];
    std::copy(theString.begin(), theString.end(), writable);
    writable[theString.size()] = '\0'; // don't forget the terminating 0

  return writable;

}
bool Ubidots_Peripheral::doSetup()
{
    m_Ubidots->setMethod(TYPE_UDP);
    m_Ubidots->setDeviceName(GetCharPtrOfStdString_WatchMemory(m_ubidots_device_name));
    m_Ubidots->setDeviceLabel(GetCharPtrOfStdString_WatchMemory(m_ubidots_device_label));
  
  return true;
}
bool Ubidots_Peripheral::doMainLoop()
{
	//SensorValue::Data_t something = m_Parameters.begin()->GetCurrentValue(); //humidity.GetCurrentValue();

	//m_Humidity_old= boost::get<double>(something);
	//m_TemperatureC = m_DHT.getTempCelcius();

	return true;
}

bool Ubidots_Peripheral::SendData(const std::vector<std::pair<std::string,double>>& dataToSend )
{
  for (auto itr = dataToSend.begin(); itr != dataToSend.end(); ++itr)
  {
      //char* is fucking stupid.
      std::string str = itr->first;
      
      m_Ubidots->add(GetCharPtrOfStdString_WatchMemory(str), itr->second); //seems to take responsibility for the char*

      m_Console->logMessage(String::format("Cell transmission: name=[%s], value=[%.2f]",str.c_str(), itr->second));
      // don't forget to free the string after finished using it   
  }

  return m_Ubidots->sendAll(); 
}

void Ubidots_Peripheral::PrintInfo()
{
  m_Console->logMessage(String::format("Token: %s",m_Ubidots->_token));
  m_Console->logMessage(String::format("Server: %s",m_Ubidots->_server));
  m_Console->logMessage(String::format("ID: %s",m_Ubidots->_pId));
  m_Console->logMessage(String::format("Name: %s",m_Ubidots->_dsName));
}
