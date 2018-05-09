#pragma once
#include "IPeripheralDevice.h"
#include "RFM69-Particle.h"

#include "ParticleWiring.h"
#include "Parameter.h"



class RFM69_Peripheral : public IPeripheralDevice
{
public:
    RFM69_Peripheral(std::shared_ptr<IConsoleOutput> console, uint8_t cs_pin, uint8_t irq_pin, uint8_t rst_pin);
   

    virtual std::string GetName() { return "RFM69";  }
    virtual void PrintInfo();
    void SendSomething(const std::string& theTextToSend);
    unsigned int GetRelayTimeInS();
    void RelayLoop();

protected:
    virtual bool doSetup();
    virtual bool doMainLoop();


private:
    RFM69 m_Radio;
    int m_Packetnum;
    uint8_t m_cs_pin;
    uint8_t m_irq_pin;
    uint8_t m_rst_pin;
    bool m_DeviceFound;
    int m_NetworkID;
    int m_NodeID;
    int m_Receiver;
};
