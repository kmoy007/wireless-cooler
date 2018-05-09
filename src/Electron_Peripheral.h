#pragma once
#include "IPeripheralDevice.h"

#include "ParticleWiring.h"
#include "Parameter.h"

class Electron_Peripheral : public IPeripheralDevice
{
public:
    Electron_Peripheral(std::shared_ptr<IConsoleOutput> console);

    virtual std::string GetName() { return "Electron";  }
    virtual void PrintInfo();

protected:
    virtual bool doSetup();
    virtual bool doMainLoop();


private:
    //DHT m_DHT;
    FuelGauge m_Fuel;
};
