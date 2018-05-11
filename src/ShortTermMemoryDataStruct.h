#pragma once

static const uint8_t MaxNameSize = 19; //32 bytes per entry
  //format is
  // 4 byte time integer
  // 8 bytes double value
  // 32 - (12) = 20 bytes for name.  Enforcing at least 1 NULL terminator - 19 bytes for name

class ShortTermMemoryDataStruct
{
public:
  ShortTermMemoryDataStruct() /*:
    unixTime(0),
    name{0},
    value(0)*/
  {
    static_assert(sizeof(ShortTermMemoryDataStruct) == 32, "WRONG SIZE - checkMaxNameSize");
  }

  void initialize()
  {
    unixTime = 0;
    value = 0;
    name.fill('\0');
  }

  static ShortTermMemoryDataStruct ConvertToShortTermMemoryDataStruct(const SensorValue& sensorValue)
  {
    ShortTermMemoryDataStruct theStruct;
    
    std::string theName = sensorValue.GetName();
    std::copy(theName.begin(), theName.end(), theStruct.name.begin());
    theStruct.name[min(theName.length(), theStruct.name.size())] = '\0';
    theStruct.value = sensorValue.GetCurrentValue();
    theStruct.unixTime = Time.now();
    
    return theStruct;
  }
  
  int unixTime;
  std::array<char, MaxNameSize+1> name; //add one for null terminator
  double value;


};