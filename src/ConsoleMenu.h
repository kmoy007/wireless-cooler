#pragma once
#include <memory>
#include <vector>
#include "IPeripheralDevice.h"
#include "Persistence.h"

class IConsoleOutput;

class ConsoleMenu
{
public:
	ConsoleMenu(std::shared_ptr<IConsoleOutput> theConsole, std::vector<std::shared_ptr<IPeripheralDevice>> theDevices, std::weak_ptr<Persistence> persistence);

	bool IsUserActiveRecently();
	void MenuLoop();



private:
	void MainMenu();
	void ListDevices();
	bool ProcessInput_ReturnFalseIfExit(std::string inputString);

void ParsePeripherals(const std::string& deviceName, std::function<void(std::shared_ptr<IPeripheralDevice>)> theFunction);


private:
	std::shared_ptr<IConsoleOutput> m_Console;
	std::vector<std::shared_ptr<IPeripheralDevice>> m_Devices;
	unsigned int m_msLastInteraction;
	int m_msUserInteractionTimeout;
	std::weak_ptr<Persistence> m_Persistence;
};
