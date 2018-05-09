#pragma once
#include "IPeripheralDevice.h"
#include "MCP9808.h"

#include "ParticleWiring.h"
#include "Parameter.h"



class MCP9808_Peripheral : public IPeripheralDevice
{
public:
    MCP9808_Peripheral(std::shared_ptr<IConsoleOutput> console, std::vector<uint8_t> pinsSetToInput, uint8_t alertPin);

    virtual std::string GetName() { return "MCP9808";  }
    virtual void PrintInfo();

protected:
    virtual bool doSetup();
    virtual bool doMainLoop();


private:
    MCP9808 m_MCP;
    std::vector<uint8_t> m_pinsSetToInput;
    uint8_t m_alertPin;
    bool m_DeviceFound;
};
