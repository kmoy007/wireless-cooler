#include "BLE_NVM_Data.h"
#include "mpack_interface.h"

#include <boost/tokenizer.hpp>
#include "string_format.h"
#include <algorithm>


const size_t g_NVMsize = 255;//size of NVMbuffer

BLE_NVM_Data::BLE_NVM_Data(std::shared_ptr<SerialAndBluetooth> sandb) :
	m_sandb(sandb)
{
}

uint8_t BLE_NVM_Data::GetHexAsByte_Single(std::string hexString)
{
    if (hexString.length() != 2)
    {
        m_sandb->logMessage(String::format("GetHexAsByte expected 2 characters, string as %d characters [%s]", hexString.length(), hexString));
        return 0;
    }
    const std::string zero = "00"; 
    
    unsigned int val = strtoul(hexString.c_str(), 0, 16);
    if ((hexString != zero) && (val == 0))
    {
        m_sandb->logMessage(String::format("GetHexAsByte couldnt convert [%s]", hexString));
    }
    return static_cast<uint8_t>(val);
}

std::vector<uint8_t> BLE_NVM_Data::GetHexStringAsBytes(std::string hexString)
{
  boost::char_separator<char> sep{"-"};
  boost::tokenizer<boost::char_separator<char>> tok{hexString, sep};
    
  std::vector<std::string> theStrings;
  for (const auto &t : tok)
  {
    theStrings.push_back(t);
  }

  std::vector<uint8_t> theResult;
  for (auto itr = theStrings.begin(); itr != theStrings.end(); ++itr)
  {
      theResult.push_back(GetHexAsByte_Single(*itr));
  }

  return theResult;
}

std::string BLE_NVM_Data::GetByteStringAsHexString(std::vector<uint8_t> theBytes)
{
    std::string resultString;
    for (auto itr = theBytes.begin(); itr != theBytes.end(); ++itr)
    {
        resultString += String::format("%02X-", *itr);
    }

    if (resultString.size() > 0)
    {
        resultString.pop_back(); //remove final '-'
    }
    return resultString;
}

void BLE_NVM_Data::PrintNVM(std::shared_ptr<IATCommandInterface> theATInterface)
{
   
    std::vector<std::string> returnStrings;
    bool result = theATInterface->ATCommand_GetResult("AT+NVMREAD=0, 255, BYTEARRAY", returnStrings);
    
    if ((!result))
    {
        m_sandb->logMessage("BLE_Peripheral::PrintNVM - readNVM call failed! Strings:");
        for (auto itr = returnStrings.begin(); itr!= returnStrings.end(); ++itr)
		{
		    m_sandb->logMessage(String::format("BLE output: %s",itr->c_str()));
		}
        return;
    }
    if (returnStrings.size() == 0)
    {
        m_sandb->logMessage("BLE_Peripheral::PrintNVM - returned strings empty!");
        return;
    }

    returnStrings.pop_back(); //remove the OK

    std::string payload;
    for (auto itr = returnStrings.begin(); itr != returnStrings.end(); ++itr)
    {
        payload += *itr;
    }

    std::vector<uint8_t> packedData = GetHexStringAsBytes(payload);

    if (packedData.size() == 0)
    {
       m_sandb->logMessage("Tried to unpack empty vector");
       return;
    }
    std::shared_ptr<nvmdata> theData = mpack_nvm::UnpackData(&packedData[0], g_NVMsize);

    if (theData)
    {
        PrintNVM(theData);
    }
    else
    {
        m_sandb->logMessage("Unpacking messagepack data FAILED!");
        //m_sandb->logMessage(String::format("BLE output: %s",itr->c_str()));
    }
}

void BLE_NVM_Data::PrintNVM(std::shared_ptr<nvmdata> dataToPrint)
{
  m_sandb->logMessage("PrintNVM:");
  static_assert(sizeof(nvmdata) == 12, "WRONG SIZE - nvmData - update printing below");
  if (!dataToPrint)
  {
      m_sandb->logMessage("ERROR!  BLE_Peripheral::PrintNVM. dataToPrint is NULL!");
      return;
  }
  m_sandb->logMessage(String::format("version: %u", dataToPrint->version));
  m_sandb->logMessage(String::format("lastWake: [%d] - %s", dataToPrint->lastWakeTime, Time.format(dataToPrint->lastWakeTime, TIME_FORMAT_DEFAULT).c_str()));
  m_sandb->logMessage(String::format("nextWake: [%d] - %s", dataToPrint->nextWakeTime, Time.format(dataToPrint->nextWakeTime, TIME_FORMAT_DEFAULT).c_str()));
  m_sandb->logMessage("---end---");
}

bool BLE_NVM_Data::UploadNVMData(uint8_t version, int thisWakeTime, int nextWakeTime, std::shared_ptr<IATCommandInterface> theATInterface)
{
    std::shared_ptr<nvmdata> theData = std::make_shared<nvmdata>();
    theData->version = version;
    theData->nextWakeTime = nextWakeTime;
    theData->lastWakeTime = thisWakeTime;

    std::vector<uint8_t> packedData;
    packedData.resize(g_NVMsize, 0);

    bool success = mpack_nvm::PackData(theData, &packedData[0], g_NVMsize);
    if (!success)
    {
       m_sandb->logMessage("Message Pack failed on this data:");
       PrintNVM(theData);
       return false;
    }

    const int chunkSize = 50;
    for (int i = 0; i < g_NVMsize; i+=chunkSize)
    {
        std::vector<uint8_t> packedDataPiece(packedData.begin() + i, packedData.begin() + std::min(i+chunkSize, static_cast<int>(g_NVMsize)));
        std::string theCommand = string_format("AT+NVMWRITE=%d, BYTEARRAY,",i);
        theCommand += GetByteStringAsHexString(packedDataPiece);
        //theCommand += 0;
          

        std::vector<std::string> returnStrings;
        bool result = theATInterface->ATCommand_GetResult(theCommand.c_str(), returnStrings);

        if (!result)
        {
            m_sandb->logMessage("BLE_Peripheral::UploadNVMData - writeNVM call FAILED!");
            return false;
        }

    }


    return true;
}