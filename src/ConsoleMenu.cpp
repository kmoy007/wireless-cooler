#include "ConsoleMenu.h"
#include <Particle.h>

#include <boost/tokenizer.hpp>
#include "ParameterHelpers.h"

ConsoleMenu::ConsoleMenu(std::shared_ptr<IConsoleOutput> theConsole, std::vector<std::shared_ptr<IPeripheralDevice>> theDevices, std::weak_ptr<Persistence> persistence) :
	m_Console(theConsole),
	m_Devices(theDevices),
	m_msLastInteraction(0),
	m_msUserInteractionTimeout(60000),
	m_Persistence(persistence)
{
	m_msLastInteraction = 0-m_msUserInteractionTimeout; //now - timeout
}

bool ConsoleMenu::IsUserActiveRecently()
{
	const int timeSinceLastUserInteraction = millis() - m_msLastInteraction;

	if (timeSinceLastUserInteraction < m_msUserInteractionTimeout)
	{
		return true;
	}
	
	std::vector<Stream*> theStreams = m_Console->GetStreams();
	for (auto itr = theStreams.begin(); itr != theStreams.end(); ++itr)
	{
		if ((*itr)->available() > 0)
		{
			return true;
		}
	}
	
	return false;
}

void ConsoleMenu::ListDevices()
{
	for (auto itr = m_Devices.begin(); itr != m_Devices.end(); ++itr)
	{
		m_Console->logMessage(String::format("%s", (*itr)->GetName().c_str()));
	}
}


void ConsoleMenu::MainMenu()
{
	m_Console->logMessage("----");
	m_Console->logMessage("command <parameter> : description");
	m_Console->logMessage("menu : this menu");
	m_Console->logMessage("list : list all devices");
	m_Console->logMessage("sensors <device> : read sensors for <device>");
	m_Console->logMessage("sensors all : read all sensors for all devices");
	m_Console->logMessage("parameters <device> : read parameters for <device>");
	m_Console->logMessage("parameters all : read all parameters for all devices");
	m_Console->logMessage("functions <device> : list functions for <device>");
	m_Console->logMessage("functions all : list all functions for all devices");
	m_Console->logMessage("function <device> <function> : call this function");
	m_Console->logMessage("info <device> : call PrintInfo for <device>");	
	m_Console->logMessage("set <device> <parameter> <value>: set the value (double)");	
	m_Console->logMessage("setflash <device> <parameter> <value>: set the value (double) AND flash it");
	m_Console->logMessage("erase <parameter>: erase it in flash");
	m_Console->logMessage(String::format("exit : exit menu (will also timout after %u seconds", m_msUserInteractionTimeout/1000));
	//m_Console->logMessage("listall : list all devices and parameters");
	m_Console->logMessage("");
}


void ConsoleMenu::MenuLoop()
{
	// Constants
	const bool echo = true;
	const char TERMINATOR = '\n';
	const size_t READ_BUF_SIZE = 64;


	char readBuf[READ_BUF_SIZE];
	size_t readBufOffset = 0;

	long lastCharTime = millis();

	bool runTheLoop = IsUserActiveRecently();
	//START OF THE LOOP

	if (runTheLoop)
	{
		MainMenu();
		while (runTheLoop)
		{
			int msTimeSinceLastChar = millis() - lastCharTime;

			if (msTimeSinceLastChar >= m_msUserInteractionTimeout)
			{
				lastCharTime = millis();
				readBuf[readBufOffset] = 0;
				Serial.printlnf("Timeout waiting for Serial.  Buffer contents: %s", readBuf);
				readBufOffset = 0;
				runTheLoop = false;
			}

			char receivedChar = 0;
			bool receivedSomething = false;

			
			std::vector<Stream*> theStreams = m_Console->GetStreams();
			for (auto itr = theStreams.begin(); itr != theStreams.end(); ++itr)
			{
				if ((*itr)->available() > 0)  //something in the buffer
				{
					receivedSomething = true;
					lastCharTime = millis();

					receivedChar = (*itr)->read();
					if (echo)
					{
						Serial.write(receivedChar);//hack for now.  BLE empties read buffer on write.
						//Serial.write('*');
					}
				}

			}
			
			
			
			if (receivedSomething)
			{
				if (receivedChar != TERMINATOR)
				{
					if (readBufOffset >= READ_BUF_SIZE)
					{
						Serial.println("buffer overflow! emptying buffer");
						readBufOffset = 0;
					}
					// Add character to buffer
					readBuf[readBufOffset++] = receivedChar;

				}
				else
				{
					// End of line character found, process line
					readBuf[readBufOffset] = 0;
					readBufOffset = 0; //reset the buffer

					std::string inputString = readBuf;
			
					inputString.erase(inputString.find_last_not_of("\t\n\v\f\r ") + 1); //remove enter

					runTheLoop = ProcessInput_ReturnFalseIfExit(inputString);
				}
			}
		}
	}
}

void ConsoleMenu::ParsePeripherals(const std::string& deviceName, std::function<void(std::shared_ptr<IPeripheralDevice>)> theFunction)
{
	if (deviceName == "all")
	{
		for (auto itr = m_Devices.begin(); itr != m_Devices.end(); ++itr)
		{
			m_Console->logMessage(String::format("Device: %s", (*itr)->GetName().c_str()));
			theFunction(*itr);
			//(*itr)->PrintInfo();
		}
	}
	else
	{
		auto theDevice = std::find_if(m_Devices.begin(), m_Devices.end(), [&deviceName](const std::shared_ptr<IPeripheralDevice>& x) { return x->GetName() == deviceName;});
		if (theDevice == m_Devices.end())
		{
			m_Console->logMessage(String::format("device not found: %s", deviceName.c_str()));	
		}
		else
		{
			theFunction(*theDevice);
			//(*theDevice)->PrintInfo();
		}
	}
}
bool ConsoleMenu::ProcessInput_ReturnFalseIfExit(std::string inputString)
{
	boost::char_separator<char> sep{" "};
	boost::tokenizer<boost::char_separator<char>> tok{inputString, sep};
		
		//for(tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
	std::vector<std::string> theTokens;
	for (const auto &t : tok)
	{
		//m_Console->logMessage(t.c_str());
		theTokens.push_back(t);
	}


	if (theTokens.size() == 1)
	{
		std::string& firstToken = *(theTokens.begin());

		if (firstToken == "exit")
		{
			m_Console->logMessage("Exiting!");
			return false;
		}
		else if (firstToken == "list")
		{
			ListDevices();
		}	 	
		else if (firstToken == "menu")
		{
			MainMenu();
		}	 				
		else
		{
			m_Console->logMessage(String::format("bad command: %s", firstToken.c_str()));	
		}
	}
	else if (theTokens.size() == 2)
	{
		std::string& firstToken = *(theTokens.begin());
		std::string& secondToken = theTokens.at(1);


		if (firstToken == "sensors")
		{
			ParsePeripherals(secondToken, [](std::shared_ptr<IPeripheralDevice> theDevice) { theDevice->PrintAllSensorValues();  });
		}	
		else if (firstToken == "parameters")
		{
			ParsePeripherals(secondToken, [](std::shared_ptr<IPeripheralDevice> theDevice) { theDevice->PrintParameters();  });
		}	
		else if (firstToken == "info")
		{
			ParsePeripherals(secondToken, [](std::shared_ptr<IPeripheralDevice> theDevice) { theDevice->PrintInfo();  });
		}		
		else if (firstToken == "functions")
		{
			ParsePeripherals(secondToken, [](std::shared_ptr<IPeripheralDevice> theDevice) { theDevice->PrintFunctions();  });
		}	
		else if (firstToken == "erase")
		{
			auto persistenceObj = m_Persistence.lock();
			if (persistenceObj)
			{
				Persistence::PersistenceDataStruct emptyDataStruct;
				strncpy(emptyDataStruct.name, secondToken.c_str(), sizeof(emptyDataStruct.name));
				emptyDataStruct.empty = true;
				if (!persistenceObj->SaveThisData(emptyDataStruct))
				{
					m_Console->logMessage(String::format("ERROR: failed erasing parameter [%s] ", emptyDataStruct.name));
				}
			}
			else
			{
				m_Console->logMessage("ERROR: failed to lock persistence object");
			}

		}
		else
		{
			m_Console->logMessage(String::format("bad command: %s ", firstToken.c_str()));
		}
	}
	else if (theTokens.size() == 3)
	{
		std::string& firstToken = *(theTokens.begin());
		std::string& secondToken = theTokens.at(1);
		std::string& thirdToken = theTokens.at(2);
		if (firstToken == "function")
		{
			auto theDevice = std::find_if(m_Devices.begin(), m_Devices.end(), [&secondToken](const std::shared_ptr<IPeripheralDevice>& x) { return x->GetName() == secondToken;});
			if (theDevice == m_Devices.end())
			{
				m_Console->logMessage(String::format("device not found: %s", secondToken.c_str()));	
			}
			else
			{
				std::shared_ptr<IPeripheralDevice> theDevice_sp = *theDevice;
				auto theFunction = std::find_if(theDevice_sp->m_Functions.begin(), theDevice_sp->m_Functions.end(), [&thirdToken](std::pair<std::string, std::function<void(std::string)>> x) { return x.first == thirdToken;});
				if (theFunction == theDevice_sp->m_Functions.end())
				{
					m_Console->logMessage(String::format("function not found: %s", thirdToken.c_str()));	
				}
				else
				{
					m_Console->logMessage(String::format("Calling Function: %s", thirdToken.c_str()));	
					theFunction->second(std::string());
				}

			}
		}
	}
	else if (theTokens.size() >= 4)
	{
		std::string& firstToken = *(theTokens.begin());
		std::string& secondToken = theTokens.at(1);
		std::string& thirdToken = theTokens.at(2);
		std::string& fourthToken = theTokens.at(3);


		if (firstToken == "function")
		{
			auto theDevice = std::find_if(m_Devices.begin(), m_Devices.end(), [&secondToken](const std::shared_ptr<IPeripheralDevice>& x) { return x->GetName() == secondToken;});
			if (theDevice == m_Devices.end())
			{
				m_Console->logMessage(String::format("device not found: %s", secondToken.c_str()));	
			}
			else
			{
				std::shared_ptr<IPeripheralDevice> theDevice_sp = *theDevice;
				auto theFunction = std::find_if(theDevice_sp->m_Functions.begin(), theDevice_sp->m_Functions.end(), [&thirdToken](std::pair<std::string, std::function<void(std::string)>> x) { return x.first == thirdToken;});
				if (theFunction == theDevice_sp->m_Functions.end())
				{
					m_Console->logMessage(String::format("function not found: %s", thirdToken.c_str()));	
				}
				else
				{
					m_Console->logMessage(String::format("Calling Function: %s with parameter [%s]", thirdToken.c_str(), fourthToken.c_str()));	
					theFunction->second(fourthToken);
				}

			}
		}
		else if ((firstToken == "set") || (firstToken == "setflash"))
		{
			auto theDevice = std::find_if(m_Devices.begin(), m_Devices.end(), [&secondToken](const std::shared_ptr<IPeripheralDevice>& x) { return x->GetName() == secondToken;});
			if (theDevice == m_Devices.end())
			{
				m_Console->logMessage(String::format("device not found: %s", secondToken.c_str()));	
			}
			else
			{
				std::shared_ptr<IPeripheralDevice> theDevice_sp = *theDevice;
				auto theParameter = std::find_if(theDevice_sp->m_Parameters.begin(), theDevice_sp->m_Parameters.end(), [&thirdToken](const Parameter& x) { return x.GetName() == thirdToken;});
				if (theParameter == theDevice_sp->m_Parameters.end())
				{
					m_Console->logMessage(String::format("parameter not found: %s", thirdToken.c_str()));	
				}
				else
				{
					double theValue = ParameterHelpers::ConvertStringToDouble(m_Console, fourthToken, 15, 0, 60*60*6); //15sec default 6hr upper limit
					theParameter->SetCurrentValue(theValue);
					if (firstToken == "setflash")
					{
						auto persistenceObj = m_Persistence.lock();
						if (persistenceObj)
						{
							Persistence::PersistenceDataStruct theDataStruct = ParameterHelpers::CreateDataStructFromParameter(*theParameter);
							if (!persistenceObj->SaveThisData(theDataStruct))
							{
								m_Console->logMessage(String::format("ERROR: failed saving parameter [%s] ", theParameter->GetName().c_str()));
							}
						}
						else
						{
							m_Console->logMessage("ERROR: failed to lock persistence object");
						}
					}
				}
			}
		}	
		else
		{
			m_Console->logMessage(String::format("bad command: %s ", firstToken.c_str()));
		}
	}
	else
	{
		m_Console->logMessage(String::format("invalid number of arguments: %d", theTokens.size()));
	}
	return true;
}
