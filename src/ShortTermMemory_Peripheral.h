#pragma once
#include "IPeripheralDevice.h"
#include "Parameter.h"

class ShortTermMemory_Peripheral : public IPeripheralDevice
{
public:
    ShortTermMemory_Peripheral(std::shared_ptr<IConsoleOutput> console);
    virtual ~ShortTermMemory_Peripheral();

    virtual std::string GetName() { return "ShortTermMemory";  }
    virtual void PrintInfo();

    void AddSensorValue(const SensorValue& sensorValue);

private:
	void ResetMemory();
protected:
    virtual bool doSetup();
    virtual bool doMainLoop();


private:
    
};
