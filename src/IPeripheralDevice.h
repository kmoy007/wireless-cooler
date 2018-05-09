#pragma once
#include "IConsoleOutput.h"
#include "Parameter.h"

#include <string>
#include <map>


class IPeripheralDevice
{
public:

  IPeripheralDevice(std::shared_ptr<IConsoleOutput> console);
  virtual ~IPeripheralDevice() {}

  bool Setup();
  bool MainLoop();
  virtual std::string GetName() = 0;

  virtual void PrintInfo() = 0;
  void PrintAllSensorValues();
  void PrintParameters();
  void PrintFunctions();

//  Parameter::Data_t GetValueByName(const std::string& parameterName);

protected:
  virtual bool doSetup() = 0;
  virtual bool doMainLoop() = 0;

protected:
  std::shared_ptr<IConsoleOutput> m_Console;

private:
  bool m_SetupDone;

public:
  std::vector<SensorValue> m_SensorValues;
  std::vector<Parameter> m_Parameters;
  std::map<std::string, std::function<void(std::string)>> m_Functions;

};
