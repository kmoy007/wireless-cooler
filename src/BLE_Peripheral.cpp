#include <cmath>
#include "BLE_Peripheral.h"

#include "Adafruit_BLE.h"
#include "BluefruitConfig.h"
#include "ScopedBLEModeSetter.h"

#define MINIMUM_FIRMWARE_VERSION   "0.7.1"

bool BLE_Peripheral::ConfigureWakeUpIRQ()
{
  if ( !m_ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    m_sandb->logMessage("ERROR : Callback with DFU Pin as IRQ requires at least 0.7.1");
    return false;
  }
  //m_sandb->logMessage("Setting BLE DFU to IRQ for waking up device.");
  if (ATCommand_PrintResult("AT+DFUIRQ=on"))
  {
    //m_sandb->logMessage("BLE name set SUCCESS");
    //ATCommand_PrintResult("ATZ");
    return true;
  }
  else
  {
   // m_sandb->logMessage("BLE name set FAILED");
    return false;
  }
  
}
bool BLE_Peripheral::SetBLEDeviceName(std::string newName)
{
  std::string currentDeviceName = GetBLEDeviceName();
  bool needToChange = !(currentDeviceName == newName);

  m_sandb->logMessage(String::format("BLE: Current Device Name [%s], new Device Name [%s], need to change [%s]",
    currentDeviceName.c_str(),
    newName.c_str(),
    needToChange ? "true" : "false"));

  if (!needToChange)
  {
    return true; //dont change. this logic means we dont need to reset, so we preserve the connection
  }
  std::string theBLEName("AT+GAPDEVNAME=");
  theBLEName += newName;

  if (ATCommand_PrintResult(theBLEName))
  {
    ATCommand_PrintResult("ATZ");
    return true;
  }
  else
  {
    m_sandb->logMessage("BLE name set FAILED");
    return false;
  }
  return false;
}

std::string BLE_Peripheral::GetBLEDeviceName()
{
  std::vector<std::string> returnStrings;
  if (ATCommand_GetResult("AT+GAPDEVNAME", returnStrings))
  {
    if (returnStrings.size() != 2)
    {
      m_sandb->logMessage(String::format("ERROR: GetBLEDeviceName unexpected number of return strings [%d]", returnStrings.size()));
    }
    else
    {
      return returnStrings.at(0);
    }
  }

  m_sandb->logMessage(F("ERROR: Failed to get BLE Device Name"));
  return "ERROR";
}

void BLE_Peripheral::FactoryReset()
{

    /* Perform a factory reset to make sure everything is in a known state */
    m_sandb->logMessage(F("Performing a factory reset: "));
    if ( ! m_ble.factoryReset() )
    {
      Serial.println(F("Couldn't factory reset"));
      m_sandb->SetBlueTooth(NULL);
    }
}

BLE_Peripheral::BLE_Peripheral(std::shared_ptr<SerialAndBluetooth> sandb, std::string deviceName) :
  IPeripheralDevice(sandb),
  m_sandb(sandb),
  m_ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST),
  m_DeviceName(deviceName)
{}

bool BLE_Peripheral::doSetup()
{

  // set the DFU pin high
  pinMode(ParticleWiring::pin_C4, OUTPUT);
  digitalWrite(ParticleWiring::pin_C4, HIGH);

 // pinMode(BLUEFRUIT_SPI_CS, OUTPUT);
  pinMode(BLUEFRUIT_SPI_IRQ, INPUT);
  //digitalWrite(ParticleWiring::pin_C4, HIGH);
  
  if ( m_ble.begin(VERBOSE_MODE) )
  {
    Serial.println(F("BT begin done!"));
    m_sandb->SetBlueTooth(shared_from_this());
  }
  else
  {
    Serial.println(F("Couldn't find Bluefruit, make sure it's in Command mode & check wiring?"));
  }

  if ( FACTORYRESET_ENABLE )
  {
    FactoryReset();
  }
  
  SetBLEDeviceName(m_DeviceName);

  ConfigureWakeUpIRQ();

  ATCommand_PrintResult("AT+HWMODELED=DISABLE"); //TurnOffStatusLED
  ATCommand_PrintResult("AT+HWGETDIETEMP");
  ATCommand_PrintResult("AT+MODESWITCHEN=ble,1"); //Enable remote AT commands (enable +++ remotely)


  /* Disable command echo from Bluefruit */
  m_ble.echo(false);

  //_sandb.logMessage("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  //ble.info();

  // Wait for connection
  
  const unsigned int msToWaitForBT = 10000;
  const unsigned int finishTime = millis() + msToWaitForBT;

  while (! m_ble.isConnected()) 
  {
    int timeToGoInMS = finishTime - millis();
    m_sandb->logMessage(String::format("Waiting %d ms for bluetooth connection for uart...", timeToGoInMS));
    
    delay(1000);
    if (timeToGoInMS < 0)
    {
      m_sandb->logMessage("Timed-out waiting for bluetooth connection for uart...");
      break;
    }
  }

  m_sandb->logMessage(String::format("Bluetooth connection? %s", m_ble.isConnected() ? "true" : "false"));

  // Set module to DATA mode
  m_sandb->logMessage( F("Bluetooth switching to DATA mode") );
  m_ble.setMode(BLUEFRUIT_MODE_DATA);

  m_Functions.insert(std::make_pair("atcommand",[this](std::string thecommand) {this->ATCommand_PrintResult(thecommand); }));
  return true;
}

bool BLE_Peripheral::doMainLoop()
{
  return true;
}

bool BLE_Peripheral::GetOutputFromBLEDevice(std::vector<std::string>& result)
{
  if (result.begin() != result.end())
  {
     m_sandb->logMessage("BLE_Peripheral::GetOutputFromBLEDevice input not empty!");
  }
  while ( m_ble.readline() ) 
  {
    result.push_back(m_ble.buffer);
    if ( !strcmp(m_ble.buffer, "OK") || !strcmp(m_ble.buffer, "ERROR")  ) break;  
  }

  if (result.begin() != result.end()) //not empty
  {
     if (result.back() == "OK")
     {
        return true;
     }
  }
  return false;
}

void BLE_Peripheral::PrintStrings(const std::vector<std::string>& returnStrings)
{
  for (auto itr = returnStrings.begin(); itr!= returnStrings.end(); ++itr)
  {
    m_sandb->logMessage(String::format("BLE output: %s",itr->c_str()));
  }
}

bool BLE_Peripheral::ATCommand_PrintResult(const std::string& command)
{
  std::vector<std::string> returnStrings;
  bool result = ATCommand_GetResult(command, returnStrings);
  PrintStrings(returnStrings);

  return result;
}

bool BLE_Peripheral::ATCommand_GetResult(const std::string& command, std::vector<std::string>& returnStrings)
{
  ScopedBLEModeSetter modeSetter(&m_ble, BLUEFRUIT_MODE_COMMAND);

  m_sandb->logMessage(String::format("BLE Sending AT command: %s",command.c_str()));
  m_ble.println(command.c_str());
  bool result = GetOutputFromBLEDevice(returnStrings);
  return result;
}

void BLE_Peripheral::UART_OutputString(const std::string& outputString)
{
  ScopedBLEModeSetter modeSetter(&m_ble, BLUEFRUIT_MODE_DATA);
  m_ble.println(outputString.c_str());

}

void BLE_Peripheral::PrintInfo()
{

//  bool v = m_ble._verbose;
//  m_ble._verbose = true;

  ATCommand_PrintResult("ATI"); 

//  m_ble._verbose = v;
}
