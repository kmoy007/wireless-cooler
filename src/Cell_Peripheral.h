#pragma once
#include "IPeripheralDevice.h"

#include "Cell_Implementation.h"



class Cell_Peripheral : public IPeripheralDevice
{
public:
    Cell_Peripheral(std::shared_ptr<IConsoleOutput> console);

    int GetTimeToNextsend_s();
    void EndCellSession();

    virtual std::string GetName() { return "Cell";  }
    virtual void PrintInfo();

    bool ConnectCell();

    double GetLastTimeToConnect_s() { return m_LastTimeToConnect; } 
    bool IsCellEnabled();

protected:
    virtual bool doSetup();
    virtual bool doMainLoop();


private:
    bool TimeToSendCell();
    unsigned int GetCellSendPeriodInS();
    double m_LastTimeToConnect;

    Cell_Implementation m_CellImplementation;
   // int m_LastCellSendSeconds;
   // unsigned long ;
};











