#include "Persistence.h"
#include <string>

Persistence::Persistence(std::shared_ptr<IConsoleOutput> console) :
		IPeripheralDevice(console)
{

}


int Persistence::GetAddressForThisData_NegativeIfNotFound(const std::string& nameToFind)
{
	int theAddress = -1;
	const int blockSize = sizeof(PersistenceDataStruct);
	int lastAddress = TotalAvailablePersistencyBytes() - blockSize;

	for (int address = 0; address < lastAddress; address += blockSize)
	{
		PersistenceDataStruct theValues;
		EEPROM.get(address, theValues);
		if (theValues.empty == false)
		{
			//this has a real value
			std::string thisEntryName = theValues.name;
			if (thisEntryName == nameToFind)
			{
				theAddress = address;
			}
		}

	}

	return theAddress;
}

int Persistence::GetNextFreeAddress_NegativeIfFull()
{
	//int theAddress = -1;
	const int blockSize = sizeof(PersistenceDataStruct);
	int lastAddress = TotalAvailablePersistencyBytes() - blockSize;

	for (int address = 0; address < lastAddress; address += blockSize)
	{
		PersistenceDataStruct theValues;
		EEPROM.get(address, theValues);
		//m_Console->logMessage(String::format("Checking: Address[%d] Empty: [%s] Name: [%s] Value: [%f]",address,theValues.empty ? "true" : "false", theValues.name, theValues.value));

		if (theValues.empty == true)
		{
			return address;
		}
	}
	return -1;
}

int Persistence::CountFreeBlocks()
{
	int counter = 0;
	const int blockSize = sizeof(PersistenceDataStruct);
	int lastAddress = TotalAvailablePersistencyBytes() - blockSize;

	for (int address = 0; address < lastAddress; address += blockSize)
	{
		PersistenceDataStruct theValues;
		EEPROM.get(address, theValues);
		if (theValues.empty == true)
		{
			counter++;
		}
	}
	return counter;
}





bool Persistence::SaveThisData(const PersistenceDataStruct& theData)
{
	int theAddress = GetAddressForThisData_NegativeIfNotFound(std::string(theData.name));

	if (theAddress < 0)
	{
		theAddress = GetNextFreeAddress_NegativeIfFull();
		if (theAddress < 0)
		{
			m_Console->logMessage(String::format("Save data failed, returned %d - memory full?", theAddress));
			return false; //memory full
		}
	}

	EEPROM.put(theAddress, theData);
	return true;
}

Persistence::PersistenceDataStruct Persistence::GetPersistenceDataStructByName(const std::string& nameToFind)
{
	Persistence::PersistenceDataStruct theStruct;

	int address = GetAddressForThisData_NegativeIfNotFound(nameToFind);
	if (address >= 0)
	{
		EEPROM.get(address, theStruct);
	}

	return theStruct;
}

void Persistence::PrintInfo()
{
	m_Console->logMessage("Persistence Helper");
	m_Console->logMessage(String::format("objects are %u bytes, total flash is %u, so we can have %u blocks", sizeof(PersistenceDataStruct), TotalAvailablePersistencyBytes(), TotalAvailablePersistencyBytes()/sizeof(PersistenceDataStruct) ));
	m_Console->logMessage(String::format("there are %d free blocks", CountFreeBlocks()));
	
	m_Console->logMessage("-----Memory Map-----");

	const int blockSize = sizeof(PersistenceDataStruct);
	int lastAddress = TotalAvailablePersistencyBytes() - blockSize;

	int counter = 0;
	for (int address = 0; address < lastAddress; address += blockSize)
	{
		PersistenceDataStruct theValues;
		EEPROM.get(address, theValues);
		m_Console->logMessage(String::format("#%02d Address[%d] Empty: [%s] Name: [%s] Value: [%f]",
			counter, address,
			theValues.empty ? "true" : "false", theValues.name, theValues.value));
		counter++;
	}
	m_Console->logMessage("----------------");

}