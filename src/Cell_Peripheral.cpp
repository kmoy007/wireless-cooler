#include "Cell_Peripheral.h"
#include "ParameterHelpers.h"

//const unsigned long m_msCellSendPeriod = 5*60*1000; //5 min
retained int m_LastCellSendSeconds = 0;
retained int failedConnects = 0;

Cell_Peripheral::Cell_Peripheral(std::shared_ptr<IConsoleOutput> console) :
  IPeripheralDevice(console),
  m_LastTimeToConnect(-1)
  //m_LastCellSendSeconds(0)
{
}

void Cell_Peripheral::EndCellSession()
{
    Particle.process();
    m_Console->logMessage(String::format("Ending Cell Session 1 of 4 - Particle: %s, Cellular connecting: %s, Cellular ready: %s", 
      Particle.connected() ? "Connected" : "NOT CONNECTED",
      Cellular.connecting() ? "Connected" : "NOT CONNECTED",
      Cellular.ready() ? "Yes" : "NO"
 
      ));
    Particle.disconnect();
    Particle.process();
    delay(500);

    m_Console->logMessage(String::format("Ending Cell Session 2 of 4 - Particle: %s, Cellular connecting: %s, Cellular ready: %s", 
      Particle.connected() ? "Connected" : "NOT CONNECTED",
      Cellular.connecting() ? "Connected" : "NOT CONNECTED",
      Cellular.ready() ? "Yes" : "NO"
    ));

    Cellular.disconnect();
    Particle.process();
    delay(500);

    m_Console->logMessage(String::format("Ending Cell Session 3 of 4 - Particle: %s, Cellular connecting: %s, Cellular ready: %s", 
      Particle.connected() ? "Connected" : "NOT CONNECTED",
      Cellular.connecting() ? "Connected" : "NOT CONNECTED",
      Cellular.ready() ? "Yes" : "NO"
    ));
    
    Cellular.off();
    Particle.process();
    delay(500);

    m_Console->logMessage(String::format("Ending Cell Session 4 of 4 - Particle: %s, Cellular connecting: %s, Cellular ready: %s", 
      Particle.connected() ? "Connected" : "NOT CONNECTED",
      Cellular.connecting() ? "Connected" : "NOT CONNECTED",
      Cellular.ready() ? "Yes" : "NO"
    ));
    m_LastCellSendSeconds=Time.now();
}

int Cell_Peripheral::GetTimeToNextsend_s()
{
    unsigned long timeToNextSend = GetCellSendPeriodInS() - (Time.now() - m_LastCellSendSeconds);
    return timeToNextSend;
}

bool Cell_Peripheral::doSetup()
{
  m_CellImplementation.ConnectionEventSetup();
  m_Parameters.push_back(Parameter("cellsendperiodseconds", 60*5)); //5mins
  m_Parameters.push_back(Parameter("cellconnecttimeoutseconds", 60*3)); //3mins
  m_Parameters.push_back(Parameter("cellactivezeroifdisable", 1)); // 1 is ON
  m_Functions.insert(std::make_pair("triggerwatchdog",[](std::string) {while(1) {} }));
  m_Functions.insert(std::make_pair("resetmodem",[this](std::string) {this->m_CellImplementation.ResetModem(); }));
  //m_Functions.insert(std::make_pair("reset",));

  SensorValue numFailures("failedConnects", [&failedConnects = failedConnects]{ return failedConnects; });
  m_SensorValues.push_back(numFailures);
  

//[](std::shared_ptr<IPeripheralDevice> theDevice) { theDevice->PrintAllSensorValues();  })

  return true;
}
bool Cell_Peripheral::doMainLoop()
{
  return true;
}

bool Cell_Peripheral::ConnectCell()
{
  if (!IsCellEnabled())
  {
     return false;
  }
  if (TimeToSendCell())
  {
    double startTime_ms = static_cast<double>(millis());
    m_Console->logMessage("******Cellular WAKE UP********");
    //Cellular.on();

    unsigned int connectStart = millis();
    Particle.connect();
    
    double connectTimeout = /*boost::get<double>*/(ParameterHelpers::GetValueByName<Parameter>("cellconnecttimeoutseconds", m_Parameters, m_Console));
    unsigned int connectTimeoutMS = static_cast<int>(connectTimeout) * 1000;
  
    while ((!Particle.connected()) && (millis() - connectStart) < connectTimeoutMS)
    {
      Particle.process();
    }
    if ((millis() - connectStart) >= connectTimeoutMS)
    {
      //TIMEOUT!
       m_Console->logMessage(String::format("CELL: Timed Out! Waited %u ms", (millis() - connectStart)));
       failedConnects++;

       //m_CellImplementation.DebugCellConnection_RebootIfYouThinkItsAGoodIdea();
    }
    
    if (!Cellular.ready())
    {
        String outputString = String::format("Connecting: %d, Ready: %d", Cellular.connecting(), Cellular.ready());
        m_Console->logMessage(outputString);
        delay(5000);
    }

    if (Cellular.ready())
    {
        m_CellImplementation.PublishEvents();
        Particle.process();
        if (Time.isValid() == false)
        {
              if (Particle.connected() == false) 
              {
                  m_Console->logMessage(String::format("CELL: Need to connect to Particle Cloud"));
                  Particle.connect();
              }

              m_Console->logMessage(String::format("CELL: Particle Cloud %s", Particle.connected() ? "Connected" : "NOT CONNECTED"));

              m_Console->logMessage(String::format("CELL: Need to sync, Time: %s   Sync Time is Valid %s", Time.timeStr().c_str(), Time.isValid() ? "YES" : "NO"));
              Particle.syncTime();
              // Wait until Electron receives time from Particle Cloud (or connection to Particle Cloud is lost)
              while (Particle.syncTimeDone() == false)
              {
                  Particle.process();
              }
              m_Console->logMessage(String::format("CELL: Synchronized, Time: %s   Sync Time is Valid %s", Time.timeStr().c_str(), Time.isValid() ? "YES" : "NO"));
        }

        m_LastTimeToConnect = (millis() - startTime_ms) / 1000.0;
        m_Console->logMessage(String::format("CELL: Time to connect cell = %.2f seconds",m_LastTimeToConnect));
        failedConnects = 0;
        delay(3000);
        return true;
    }
    else
    {
        failedConnects++;
        m_Console->logMessage("Cellular connection NOT READY");
    }

   // EndCellSession();
  }
  return false;
}
unsigned int Cell_Peripheral::GetCellSendPeriodInS()
{
    double timeToSleepDouble = /*boost::get<double>*/(ParameterHelpers::GetValueByName<Parameter>("cellsendperiodseconds", m_Parameters, m_Console));
    unsigned int cellSendPeriodS = static_cast<int>(timeToSleepDouble);
    return cellSendPeriodS;
}

bool Cell_Peripheral::IsCellEnabled()
{
    double cellenabledindouble = /*boost::get<double>*/(ParameterHelpers::GetValueByName<Parameter>("cellactivezeroifdisable", m_Parameters, m_Console));
    if (cellenabledindouble < 0.5)
    {
      return false;
    }
    return true;
}

bool Cell_Peripheral::TimeToSendCell()
{
  if (!Time.isValid())
  {
      m_Console->logMessage("CELL:  First call, Time not synchronized - need to connect!");
      return true; //first send
  }
  
  if (m_LastCellSendSeconds == 0)
  {
    m_Console->logMessage("CELL:  First call, m_LastCellSendSeconds = 0, time to send");
    return true; //first send
  }

  const int now = Time.now();
  bool IsTimeToSend = ((now - m_LastCellSendSeconds) > static_cast<int>(GetCellSendPeriodInS()));
  m_Console->logMessage(String::format("CELL: Now: %u, LastSend: %u IsTimeToSend? %s", now, m_LastCellSendSeconds, IsTimeToSend ? "YES" : "NO"));

  return IsTimeToSend;
}


void Cell_Peripheral::PrintInfo()
{
  CellularData data;
  Cellular.getDataUsage(data);

  
  String outputString = String::format("CID: %d SESSION TX: %d RX: %d TOTAL TX: %d RX: %d",
                           data.cid,
                           data.tx_session, data.rx_session,
                           data.tx_total, data.rx_total);

  m_Console->logMessage(outputString);
  CellularSignal sig = Cellular.RSSI();

  outputString = String::format("RSSI %d -dBm    QUAL %d dB", sig.rssi, sig.qual);
  m_Console->logMessage(outputString); 
}
