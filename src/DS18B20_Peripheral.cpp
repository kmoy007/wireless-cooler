#include "DS18B20_Peripheral.h"

#include <OneWire.h>
#include "string_format.h"

const int maxSensors = 8;

DS18B20_Peripheral::DS18B20_Peripheral(std::shared_ptr<IConsoleOutput> console, uint8_t ds18b20_pin, bool something) :
  IPeripheralDevice(console),
  m_DS18B20(ds18b20_pin, false)
{

}

bool DS18B20_Peripheral::doSetup()
{
  m_DS18B20.resetsearch();                 // initialise for sensor search

  std::array<uint8_t,8> sensorAddress;

  int counter = 0;
  while(m_DS18B20.search(sensorAddress.data()))
  {
      counter++;
      m_Addresses.push_back(sensorAddress);
      if (counter >= maxSensors)
      {
        m_Console->logMessage("TOO MANY SEARCHES FOR SENSORS!");
      }
  } 
   

  String outputString = String::format("%s found %d temperature sensors", GetName().c_str(), m_Addresses.size());
  m_Console->logMessage(outputString);
  
  for (auto itr = m_Addresses.begin(); itr != m_Addresses.end(); ++itr)
  {
      outputString = String("Device Address: ");
      outputString += GetAddressAsString(*itr);
      m_Console->logMessage(outputString);
  }

  m_TemperatureC = ReadSensorValues();

 // m_Console->logMessage(string_format("m_TemperatureC has [%d] entries", m_TemperatureC.size().c_str());
  for (auto itr = m_TemperatureC.begin(); itr != m_TemperatureC.end(); ++itr)
  {
      std::string theNameLocal = itr->first;
      //m_Console->logMessage(string_format("(1) Temp : [%s], value [%f]", theNameLocal.c_str(), itr->second).c_str());
      SensorValue temperature(itr->first, [&m_Console = m_Console, theNameLocal, &m_TemperatureC = m_TemperatureC]
      {  
          auto pos = m_TemperatureC.find(theNameLocal);

          if (pos == m_TemperatureC.end()) 
          {
              m_Console->logMessage(string_format("WARNING!  Couldnt find in array [%s]", theNameLocal.c_str()).c_str());
              return -997.0;
          } 
          else 
          {
              return pos->second;
          }
      }); //end of lambda

      m_SensorValues.push_back(temperature);
  }

  //SensorValue temperature("temperature", [=]{ return m_TemperatureC; });
  return true;
}

String DS18B20_Peripheral::GetAddressAsString(std::array<uint8_t,8>& theAddress)
{
  String outputString;
  for (auto itr2 = theAddress.begin(); itr2 != theAddress.end(); ++itr2)
  {
    outputString += String::format("%02X",(*itr2));
    outputString += String(" ");
  }
  
  if (OneWire::crc8(theAddress.data(), 7) != theAddress.at(7) )
  {
      outputString += (" - CRC is not valid!");
  }
  return outputString;
}

std::map<std::string, double> DS18B20_Peripheral::ReadSensorValues()
{
    std::map<std::string, double> temperatures;
    SINGLE_THREADED_BLOCK() 
    {
      const int      MAXRETRY          = 4;

      int sensorNumber = 0;
      for (auto itr = m_Addresses.begin(); itr != m_Addresses.end(); ++itr)
      {
          double theTempValue = -999;
          int   i = 0;
          do {
            theTempValue = m_DS18B20.getTemperature(itr->data());
          } while (!m_DS18B20.crcCheck() && MAXRETRY > i++);

          if (i >= MAXRETRY) 
          {
            theTempValue = NAN;
            m_Console->logMessage("DS18B20: Invalid reading");
          }
          else
          {
            std::string name = string_format("temperature_%d",sensorNumber);
            temperatures.insert(std::make_pair(name,theTempValue));
          }
          
        sensorNumber++;
      }
    }

    return temperatures;

}

bool DS18B20_Peripheral::doMainLoop()
{
  m_TemperatureC = ReadSensorValues();
  return true;
}

void DS18B20_Peripheral::PrintInfo()
{
  String outputString = String::format("%s has %d temperature sensors", GetName().c_str(), m_Addresses.size());
  m_Console->logMessage(outputString);
  for (auto itr = m_Addresses.begin(); itr != m_Addresses.end(); ++itr)
  {
      outputString = String("Device Address: ");
      outputString += GetAddressAsString(*itr);
      outputString += "chip type: ";
      outputString += String::format("%s",m_DS18B20.getChipName(itr->data()));
      outputString += " power supply: ";
      outputString += String(m_DS18B20.readPowerSupply(itr->data()));
      m_Console->logMessage(outputString);
  }


  std::map<std::string, double> theSensorValues = ReadSensorValues();
  for (auto itr = theSensorValues.begin(); itr != theSensorValues.end(); ++itr)
  {
    outputString = String::format("%s = %.2f", itr->first.c_str(), itr->second);
    m_Console->logMessage(outputString);
  }

}
