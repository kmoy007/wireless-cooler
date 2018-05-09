#pragma once
#include "IPeripheralDevice.h"
#include <Adafruit_DHT.h>

#include "ParticleWiring.h"
#include "Parameter.h"

class DHT_Peripheral : public IPeripheralDevice
{
public:
    DHT_Peripheral(std::shared_ptr<IConsoleOutput> console, uint8_t dht_pin, uint8_t dht_type);

    virtual std::string GetName() { return "DHT";  }
    virtual void PrintInfo();

protected:
    virtual bool doSetup();
    virtual bool doMainLoop();


private:
    DHT m_DHT;
};
