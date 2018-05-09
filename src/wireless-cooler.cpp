//MAIN FILE
//#include "Particle.h"

//#include "Parameter.h"
#include "ParticleWiring.h"


#include <string>
#include <sstream>


//#include "DHT_Peripheral.h"
#include "DS18B20_Peripheral.h"
#include "BLE_Peripheral.h"
#include "Cell_Peripheral.h"
#include "Electron_Peripheral.h"
#include "Persistence.h"
#include "Ubidots_Peripheral.h"
#include "PwrMgmt_Peripheral.h"
#include "MCP9808_Peripheral.h"
#include "RFM69_Peripheral.h"
#include <math.h>

#include <memory>
#include "ConsoleMenu.h"

#include "ParameterHelpers.h"


#include "SerialAndBluetooth.h"

#include "string_format.h"
#include "Cell_Implementation.h" //for watchdog
//#define BOOST_EXCEPTION_DISABLE 1

//#include "SerialUI.h"
#define TOKEN "BBFF-ZRKJR6drk5PdBiad4PLgGt88HVjVnr"  // Add here your Ubidots TOKEN
#define DEVICE_NAME "U270 MODULE"
#define DEVICE_LABEL "U270module"

bool ISRELAY = false;

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);



retained int watchdogCounter = 0;

void watchdogTimeoutFunction()
{
    watchdogCounter++;

    // This isn't quite safe; connectionEventAdd should only be called from the main loop thread,
    // but since by definition the main loop thread is stuck when the app watchdog fires, this is
    // probably not that unsafe. (The application watchdog runs in a separate thread.)
    System.reset();
}

ApplicationWatchdog wd(600000, watchdogTimeoutFunction);



int counter;



std::shared_ptr<SerialAndBluetooth> _sandb;
std::shared_ptr<Cell_Peripheral> cell;
std::shared_ptr<Electron_Peripheral> electron;
std::shared_ptr<Persistence> persistence;
std::shared_ptr<Ubidots_Peripheral> ubidots;
std::shared_ptr<RFM69_Peripheral> rfm69;

std::vector<std::shared_ptr<IPeripheralDevice>> _theDevices;


void setup() {
    System.enableFeature(FEATURE_RETAINED_MEMORY);
    LEDStatus status;
    status.off(); // Turns the LED off
    status.setPriority(LED_PRIORITY_IMPORTANT);
    status.setActive(true); // Activates status, needed to do the step above


    //pinMode(ParticleWiring::pin_D1, INPUT);
    std::shared_ptr<DS18B20_Peripheral> ds18b20;
   // std::shared_ptr<DHT_Peripheral> dht;
    std::shared_ptr<BLE_Peripheral> ble;
    std::shared_ptr<PwrMgmt_Peripheral> pwrmgmt;
    std::shared_ptr<MCP9808_Peripheral> mcp9808;
    
    delay(2000);




    _sandb = std::make_shared<SerialAndBluetooth>();
    counter = 0;
    Serial.begin(115200);




    _sandb->logMessage(F("Creating PwrMgmt module")); //MUST ALWAYS BE FIRST IN THE
    pwrmgmt = std::make_shared<PwrMgmt_Peripheral> (_sandb, ParticleWiring::pin_C1);
    _theDevices.push_back(pwrmgmt);

    _sandb->logMessage(F("Creating electron parameters"));
    electron = std::make_shared<Electron_Peripheral>(_sandb);
    _theDevices.push_back(electron);

    _sandb->logMessage(F("Creating ble module"));
    ble = std::make_shared<BLE_Peripheral>(_sandb, DEVICE_NAME);
    _theDevices.push_back(ble);
    
    _sandb->logMessage(F("Creating ds18b20 module"));
    ds18b20 = std::make_shared<DS18B20_Peripheral> (_sandb, ParticleWiring::pin_D6, true);
    _theDevices.push_back(ds18b20);
    

    std::vector<uint8_t> mcp9808pinsToSetInput;
    mcp9808pinsToSetInput.push_back(ParticleWiring::pin_B5);
    mcp9808pinsToSetInput.push_back(ParticleWiring::pin_B4);
    mcp9808pinsToSetInput.push_back(ParticleWiring::pin_B3);
   
    _sandb->logMessage(F("Creating mcp9808 module"));
    mcp9808 = std::make_shared<MCP9808_Peripheral> (_sandb, mcp9808pinsToSetInput, ParticleWiring::pin_B2);
    _theDevices.push_back(mcp9808);

    _sandb->logMessage(F("Creating rfm69 module"));
    rfm69 = std::make_shared<RFM69_Peripheral> (_sandb, ParticleWiring::pin_D5, ParticleWiring::pin_C0, ParticleWiring::pin_C3);
    _theDevices.push_back(rfm69);
    
    _sandb->logMessage(F("Creating cell module"));
    cell = std::make_shared<Cell_Peripheral> (_sandb);
    _theDevices.push_back(cell);

    _sandb->logMessage(F("Creating ubidots module"));
    ubidots = std::make_shared<Ubidots_Peripheral> (_sandb, TOKEN, DEVICE_NAME, DEVICE_LABEL);
    _theDevices.push_back(ubidots);



    
    _sandb->logMessage(F("Creating persistence module"));
    persistence = std::make_shared<Persistence> (_sandb);
    _theDevices.push_back(persistence);


    
    
    
    for (std::vector<std::shared_ptr<IPeripheralDevice>>::iterator itr = _theDevices.begin(); itr != _theDevices.end(); ++itr)
    {
        _sandb->logMessage(String::format("Module %s calling setup()",(*itr)->GetName().c_str()));
        (*itr)->Setup();
    }


    delay(1000);


}

void loop()
{
    wd.checkin();
    String outputString("");

    counter = counter + 1;
    _sandb->logMessage(String::format("Wake up #%d, Time %s, watchdogCounter %d", counter, Time.timeStr().c_str(), watchdogCounter));
   // _sandb->logMessage(Time.timeStr());
    

    for (auto itr = _theDevices.begin(); itr != _theDevices.end(); ++itr)
    {
        for (auto itr2 = (*itr)->m_Parameters.begin(); itr2 != (*itr)->m_Parameters.end(); ++itr2)
        {
            
            if (ParameterHelpers::LoadParameterFromFlash((*itr2), persistence))
            {
                _sandb->logMessage(String::format("Loaded From Flash: %s %s %0.2f - Hardcoded value of %0.2f", 
                    (*itr)->GetName().c_str(), 
                    itr2->GetName().c_str(), 
                    (itr2->GetCurrentValue()), //boost::get<double>
                    (itr2->GetHardCodedDefault())//boost::get<double>
                    ));
            }
        }
    }
    


    double lowPowerFloat = /*boost::get<double>*/(ParameterHelpers::GetValueByName<Parameter>("lowpowermode", electron->m_Parameters, _sandb));
  
    bool _inLowPowerMode = 0;
    if (lowPowerFloat < 0.5)
    {
        _inLowPowerMode = false;
    }
    else
    {
        _inLowPowerMode = true;
    }
    
    _sandb->logMessage("Sensor Readings:");

    for (std::vector<std::shared_ptr<IPeripheralDevice>>::iterator itr = _theDevices.begin(); itr != _theDevices.end(); ++itr)
    {
        (*itr)->MainLoop();

        std::vector<SensorValue> theSensorValues = (*itr)->m_SensorValues;
        for (std::vector<SensorValue>::iterator itr2 = theSensorValues.begin(); itr2 != theSensorValues.end(); ++itr2)
        {
            outputString = String::format("%s:  %s = %.2f", (*itr)->GetName().c_str(), itr2->GetName().c_str(), /*boost::get<double>*/(itr2->GetCurrentValue()));
            _sandb->logMessage(outputString);
        }
   }

    if (!ISRELAY)//SEND OVER RADIO - TEMPORARY
    {
        rfm69->SendSomething("Sensor Readings:");
        for (std::vector<std::shared_ptr<IPeripheralDevice>>::iterator itr = _theDevices.begin(); itr != _theDevices.end(); ++itr)
        {
            std::vector<SensorValue> theSensorValues = (*itr)->m_SensorValues;
            for (std::vector<SensorValue>::iterator itr2 = theSensorValues.begin(); itr2 != theSensorValues.end(); ++itr2)
            {
                outputString = String::format("%s:  %s = %.2f", (*itr)->GetName().c_str(), itr2->GetName().c_str(), /*boost::get<double>*/(itr2->GetCurrentValue()));
                rfm69->SendSomething(std::string(outputString.c_str()));
            }
       }
       rfm69->SendSomething("------END------");
    }

    std::vector<std::pair<std::string,double>> dataToSend;

        for (auto itr = _theDevices.begin(); itr != _theDevices.end(); ++itr)
        {
            for (auto itr2 = (*itr)->m_SensorValues.begin(); itr2 != (*itr)->m_SensorValues.end(); ++itr2)
            {
                dataToSend.push_back(std::make_pair(itr2->GetName(), /*boost::get<double>*/(itr2->GetCurrentValue())));
            }
        }

    ConsoleMenu menu(_sandb, _theDevices, persistence);
    if (ISRELAY)
    {
        unsigned int relayTimeStop = (rfm69->GetRelayTimeInS() * 1000) + millis();
        _sandb->logMessage(String::format("RFM69: START Relay Mode for %d seconds", rfm69->GetRelayTimeInS()));
        while (millis() < relayTimeStop)
        {
            rfm69->RelayLoop();
            menu.MenuLoop();
        }
        _sandb->logMessage("RFM69: END Relay Mode");
    }
    else
    {
        menu.MenuLoop(); //run it once
    }
   
   
    if (cell->ConnectCell())
    {
        //float lowPowerValueFromInternet = client.getValueWithDatasource(DEVICE_LABEL, VARIABLE_LOW_POWER_MODE);

       
        
        //ubidots.add("devicepowerstate", _inLowPowerMode);


        if (Particle.publish("wake-up cell", String::format("%.2f seconds to wake", cell->GetLastTimeToConnect_s()).c_str()) == true)
        {
            _sandb->logMessage("Particle Event Publish Successful");
        }
        else
        {
            _sandb->logMessage("Particle Event Publish FAILED");
        }

        dataToSend.push_back(std::make_pair("timeToConnect", cell->GetLastTimeToConnect_s()));
        dataToSend.push_back(std::make_pair("watchdogCounter", static_cast<double>(watchdogCounter)));

        bool successfulUbiDots = ubidots->SendData(dataToSend); 
        _sandb->logMessage(String::format("Ubidots send: %s", successfulUbiDots ? "Success" : "FAILED!"));
        Particle.process();
        delay(5000); //wait to send or something?
        cell->EndCellSession();
    }
    
    if (cell->IsCellEnabled())
    {
        long timeToNextSend = cell->GetTimeToNextsend_s();
        outputString = String::format("Next cell transmission in %d s", timeToNextSend);
        _sandb->logMessage(outputString);
    }
    else
    {
        _sandb->logMessage("Cell function is disabled - parameter is called cellactivezeroifdisable");
    }
    



    if (!ISRELAY)
    {
        if (_inLowPowerMode)
        {
            //int timeToSleepInSeconds = 60*1;
            double timeToSleepDouble = /*boost::get<double>*/(ParameterHelpers::GetValueByName<Parameter>("deepsleeptimeseconds", electron->m_Parameters, _sandb));
            
            int timeToSleepInSeconds = static_cast<int>(timeToSleepDouble);
            outputString = String::format("Going into deep sleep for %ds", timeToSleepInSeconds);
            _sandb->logMessage(outputString);
            _sandb->logMessage("---------------------");
            System.sleep(SLEEP_MODE_DEEP, timeToSleepInSeconds);
            //System.sleep(timeToSleepInSeconds);
            delay(timeToSleepInSeconds*1000);
        }
        else
        {
            double timeToSleepDouble = /*boost::get<double>*/(ParameterHelpers::GetValueByName<Parameter>("lightsleeptimeseconds", electron->m_Parameters, _sandb));
            int timeToSleepInSeconds = static_cast<int>(timeToSleepDouble);
           // int timeToSleepInSeconds = 5;
            outputString = String::format("Going into light sleep for %ds", timeToSleepInSeconds);
            _sandb->logMessage(outputString);
            _sandb->logMessage("---------------------");
            System.sleep(ParticleWiring::pin_D1,FALLING,timeToSleepInSeconds);
        }
    }


}
