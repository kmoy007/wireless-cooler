#include "ParticleWiring.h"
#include "Parameter.h"
#include "RFM69_Peripheral.h"
#include "RFM69_ATC.h"
#include "RFM69registers.h"
#include "ParameterHelpers.h"


//Match frequency to the hardware version of the radio on your Feather
#define FREQUENCY     RF69_433MHZ
//#define FREQUENCY     RF69_868MHZ
//#define FREQUENCY      RF69_915MHZ
#define ENCRYPTKEY     "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HCW    true // set to 'true' if you are using an RFM69HCW module

//*********************************************************************************************

//int16_t packetnum = 0;  // packet counter, we increment per xmission


RFM69_Peripheral::RFM69_Peripheral(std::shared_ptr<IConsoleOutput> console, uint8_t cs_pin, uint8_t irq_pin, uint8_t rst_pin) :
  IPeripheralDevice(console),
  m_Radio(cs_pin, irq_pin, IS_RFM69HCW, irq_pin),  
  m_Packetnum(0),
  m_cs_pin(cs_pin),
  m_irq_pin(irq_pin),
  m_rst_pin(rst_pin),
  m_DeviceFound(false),
  m_NetworkID(100),
  m_NodeID(2), //tx = 2
  m_Receiver(1) 
{
  
}


bool RFM69_Peripheral::doSetup()
{
  pinMode(m_rst_pin, OUTPUT);
  digitalWrite(m_rst_pin, HIGH);
  delay(100);
  digitalWrite(m_rst_pin, LOW);
  delay(100);
  
    // Initialize radio
  m_Console->logMessage("RFM69 Initializing");
  
  if (!m_Radio.initialize(FREQUENCY,m_NodeID,m_NetworkID))
  {
      m_Console->logMessage("FAILED TO INITIALIZE RADIO");
      return false;
  }
  m_DeviceFound = true;

  if (IS_RFM69HCW) {
    m_Radio.setHighPower();    // Only for RFM69HCW & HW!
  }
  

  m_Console->logMessage("RFM69: Setting bitrate, power and encrypt key");
  // To improve distance set a lower bit rate. Most libraries use 55.55 kbps as default
  // See https://lowpowerlab.com/forum/moteino/rfm69hw-bit-rate-settings/msg1979/#msg1979
  // Here we will set it to 9.6 kbps instead 
  m_Radio.writeReg(0x03,0x0D); //set bit rate to 9k6
  m_Radio.writeReg(0x04,0x05);
  
  m_Radio.setPowerLevel(20); // power output ranges from 0 (5dBm) to 31 (20dBm)
                          // Note at 20dBm the radio sources up to 130 mA! 
                         // Selecting a power level between 10 and 15 will use ~30-44 mA which is generally more compatible with Photon power sources
                        // As reference, power level of 10 transmits successfully at least 300 feet with 0% packet loss right through a home, sufficient for most use
    
  m_Radio.encrypt(ENCRYPTKEY);

  m_Parameters.push_back(Parameter("relaymodeseconds", 60*5)); //5mins
  m_Parameters.push_back(Parameter("powerlevel0to31", 20)); //20 is quite a lot

  return true;
}
bool RFM69_Peripheral::doMainLoop()
{
  int powerlevel = static_cast<int>(ParameterHelpers::GetValueByName<Parameter>("powerlevel0to31", m_Parameters, m_Console));
  m_Console->logMessage(String::format("RFM69: setting power level to [%d]", powerlevel));
  m_Radio.setPowerLevel(powerlevel);
  //SendSomething("alive");
	//SensorValue::Data_t something = m_Parameters.begin()->GetCurrentValue(); //humidity.GetCurrentValue();

	//m_Humidity_old= boost::get<double>(something);
	//m_TemperatureC = m_DHT.getTempCelcius();

	return true;
}

unsigned int RFM69_Peripheral::GetRelayTimeInS()
{
    double relayTimeDouble = /*boost::get<double>*/(ParameterHelpers::GetValueByName<Parameter>("relaymodeseconds", m_Parameters, m_Console));
    unsigned int relayTimeS = static_cast<unsigned int>(relayTimeDouble);
    return relayTimeS;
}

void RFM69_Peripheral::RelayLoop()
{
  while (m_Radio.receiveDone())
  {
      m_Console->logMessage(String::format("RFM69: [%d] %s [RX_RSSI:%d]", m_Radio.SENDERID, (char*)m_Radio.DATA, m_Radio.RSSI));
  }
  //m_Radio.receiveDone();
}

void RFM69_Peripheral::SendSomething(const std::string& theTextToSend)
{
  if (!m_DeviceFound)
  {
      m_Console->logMessage("RFM69 tried to send but device DOESNT EXIST");
  }

    m_Console->logMessage(String::format("RFM69 [%d to %d #%d] : %s", m_NodeID, m_Receiver, m_Packetnum++, theTextToSend.c_str()));
  //send the message
    m_Radio.send(m_Receiver, theTextToSend.c_str(), theTextToSend.length());//target node Id, message as string or byte array, message length
    m_Radio.receiveDone(); //put radio in RX mode
}

void RFM69_Peripheral::PrintInfo()
{
  m_Console->logMessage(String::format("RFM69 found: %s", m_DeviceFound ? "yes" : "no"));
  m_Console->logMessage(String::format("RFM69: Frequency Set to %d MHz", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915));
//  m_Console->logMessage(String::format());
//  m_Console->logMessage(String::format());
//  m_Console->logMessage(String::format());
     
}
