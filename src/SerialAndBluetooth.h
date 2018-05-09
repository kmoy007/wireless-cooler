#pragma once
#include "IConsoleOutput.h"
//#include "Adafruit_BLE.h"
//#include "Adafruit_BluefruitLE_SPI.h"
#include "IBLEInterface.h"
#include <memory>

class SerialAndBluetooth : public IConsoleOutput
{
  public:
  SerialAndBluetooth() 
  {
  }

  void SetBlueTooth(std::shared_ptr<IBLEInterface> pBlueTooth)
  {
    Serial.printlnf("BT attached") ;
    m_BLE = pBlueTooth;
  }

  void printString(const String& theString)
  {
    if (!m_BLE)
    {
      Serial.print("BT not set. ");
    }
    else if(m_BLE->IsConnectedToMaster() == false)
    {
      Serial.print("No BT. ");
    }
    else
    {
      m_BLE->UART_OutputString(theString.c_str());
    }

    Serial.println(theString);
  }

  virtual void logMessage(const String& theString)
  {
    String outputString;
    outputString = String::format("%07u :", static_cast<unsigned long>(millis()/1000));

    outputString += theString;
    printString(outputString);

  }

 /* virtual void logMessage(const std::string& theString)
  {
    String outputString(theString.c_str());
    logMessage(outputString);
  }
*/
  virtual std::vector<Stream*> GetStreams()
  {
     std::vector<Stream*> theStreams;
     theStreams.push_back(&Serial);
     if (m_BLE)
     {
        theStreams.push_back(m_BLE->GetRawBLEPtr());
     }

     return theStreams;
  }

private:
  std::shared_ptr<IBLEInterface> m_BLE;

};
