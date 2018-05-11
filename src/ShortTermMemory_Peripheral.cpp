#include "ShortTermMemory_Peripheral.h"

#include "ParticleWiring.h"
#include "Parameter.h"
//#include <boost/variant.hpp>
//#include <boost/circular_buffer.hpp>
#include "ShortTermMemory.h"
#include "ShortTermMemoryDataStruct.h"


const unsigned int SHORT_TERM_MEMORY_MAGIC = 0x5c39d415; //how to tell if we need to initalize.
retained unsigned int RetainedShortTermMagicNumber;
retained ShortTermMemory<ShortTermMemoryDataStruct> RetainedShortTermMemory;

ShortTermMemory_Peripheral::ShortTermMemory_Peripheral(std::shared_ptr<IConsoleOutput> console) :
  IPeripheralDevice(console)
{
   static_assert(ShortTermMemory<ShortTermMemoryDataStruct>::NumEntries == 64, "WRONG Number of entries in ShortTermMemory");
   if (RetainedShortTermMagicNumber != SHORT_TERM_MEMORY_MAGIC)
   {
     m_Console->logMessage("ShortTermMemory: first time run detected, initializing the memory");
     ResetMemory();
     RetainedShortTermMagicNumber = SHORT_TERM_MEMORY_MAGIC;
   }

   m_Functions.insert(std::make_pair("initializememory",[](std::string) { RetainedShortTermMemory.initialize(); }));
}

ShortTermMemory_Peripheral::~ShortTermMemory_Peripheral()
{

}

void ShortTermMemory_Peripheral::ResetMemory()
{
  RetainedShortTermMemory.initialize();
}

void ShortTermMemory_Peripheral::AddSensorValue(const SensorValue& sensorValue)
{
  ShortTermMemoryDataStruct theStruct = ShortTermMemoryDataStruct::ConvertToShortTermMemoryDataStruct(sensorValue);
  RetainedShortTermMemory.push_head(theStruct);

    //const std::string& GetName() const;
    //virtual Data_t GetCurrentValue() const = 0;
}

bool ShortTermMemory_Peripheral::doSetup()
{
 
  return true;
}
bool ShortTermMemory_Peripheral::doMainLoop()
{
	//SensorValue::Data_t something = m_Parameters.begin()->GetCurrentValue(); //humidity.GetCurrentValue();

	//m_Humidity_old= boost::get<double>(something);
	//m_TemperatureC = m_DHT.getTempCelcius();

	return true;
}

void ShortTermMemory_Peripheral::PrintInfo()
{
  m_Console->logMessage("ShortTermMemory device");
  m_Console->logMessage(String::format("Contains %d entries from a possible capacity of %d:", RetainedShortTermMemory.size(), RetainedShortTermMemory.capacity()));
  
  for (unsigned int index = 0; index < RetainedShortTermMemory.capacity(); index++)
  {
    ShortTermMemoryDataStruct theStruct;
    bool isInUse = RetainedShortTermMemory.peek(index, theStruct);

    std::string tempNameString(std::begin(theStruct.name), std::end(theStruct.name));
    std::string leaderString;
    if (isInUse)
    {
      leaderString = "inuse\t";
    }
    else
    {
      leaderString = "-----\t";
    }
    if (index == RetainedShortTermMemory.GetHeadIndex()) { leaderString = "head \t"; }
    if (index == RetainedShortTermMemory.GetTailIndex()) { leaderString = "tail \t"; }

    m_Console->logMessage(String::format("[%u] %s Time[%s] Name[%s] Value[%.2f]", 
      index,
      leaderString.c_str(),
      Time.format(theStruct.unixTime, TIME_FORMAT_ISO8601_FULL).c_str(),
      tempNameString.c_str(),
      theStruct.value));
  }
  


}
