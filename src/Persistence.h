#pragma once
#include "Particle.h"
#include <vector>
#include "IPeripheralDevice.h"
//#include "Parameter.h"

class Persistence : public IPeripheralDevice
{
public:
	static const uint8_t MaxNameSize = 23;
		//2048 bytes / 64 entries = 32 bytes/entry
		//format is
		// 1 byte used boolean
		// 8 bytes double value
		// 32 - (9) = 23 bytes for name.  Enforcing at least 1 NULL terminator - 22 bytes for name


	struct PersistenceDataStruct
	{
		PersistenceDataStruct() :
			empty(true),
			name{0},
			value(0)
			
		{
			static_assert(sizeof(PersistenceDataStruct) == 32, "WRONG SIZE - checkMaxNameSize");
		}

		bool empty;  //the device erases flash to FF
		
		char name[MaxNameSize];// = {0};
		double value;


	};

public:
	Persistence(std::shared_ptr<IConsoleOutput> console);


	virtual std::string GetName() { return "persistence"; }

	virtual void PrintInfo();

protected:
  	virtual bool doSetup() { return true; };
  	virtual bool doMainLoop() { return true; };

public:

	int GetNextFreeAddress_NegativeIfFull();
	bool SaveThisData(const PersistenceDataStruct& theData);
	int GetAddressForThisData_NegativeIfNotFound(const std::string& nameToFind);
	PersistenceDataStruct GetPersistenceDataStructByName(const std::string& nameToFind);

private:
	int CountFreeBlocks();
	unsigned int TotalAvailablePersistencyBytes() { return EEPROM.length(); }

};