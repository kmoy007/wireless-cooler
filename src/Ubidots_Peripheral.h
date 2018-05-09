#pragma once
#include "IPeripheralDevice.h"

#include "ParticleWiring.h"
#include "Parameter.h"

#include "Ubidots.h"
#include <memory>

class Ubidots_Peripheral : public IPeripheralDevice
{
public:
    Ubidots_Peripheral(std::shared_ptr<IConsoleOutput> console, 
    	const std::string& ubidots_token, 
      	const std::string& ubidots_device_name, 
      	const std::string& ubidots_device_label);

    virtual ~Ubidots_Peripheral();

    virtual std::string GetName() { return "Ubidots";  }
    virtual void PrintInfo();

    bool SendData(const std::vector<std::pair<std::string,double>>& dataToSend );


protected:
    virtual bool doSetup();
    virtual bool doMainLoop();

private:
	char * GetCharPtrOfStdString_WatchMemory(const std::string& theString);

private:
    std::unique_ptr<Ubidots> m_Ubidots;
    
    std::string m_ubidots_device_label;
    std::string m_ubidots_device_name;

};
