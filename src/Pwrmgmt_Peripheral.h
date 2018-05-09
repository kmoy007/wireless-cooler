#pragma once
#include "IPeripheralDevice.h"

#include "ParticleWiring.h"
#include "Parameter.h"

class PwrMgmt_Peripheral : public IPeripheralDevice
{
public:
    PwrMgmt_Peripheral(std::shared_ptr<IConsoleOutput> console, uint8_t power_pin);
    virtual ~PwrMgmt_Peripheral();

    virtual std::string GetName() { return "PwrMgmt";  }
    virtual void PrintInfo();

protected:
    virtual bool doSetup();
    virtual bool doMainLoop();


private:
    const uint8_t m_pin;
};
