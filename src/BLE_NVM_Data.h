#pragma once
#include <memory>
#include "SerialAndBluetooth.h"
#include "IATCommandInterface.h"
#include "nvmdata.h"

class BLE_NVM_Data
{
public:
	BLE_NVM_Data(std::shared_ptr<SerialAndBluetooth> sandb);
	bool UploadNVMData(uint8_t version, int thisWakeTime, int nextWakeTime, std::shared_ptr<IATCommandInterface> theATInterface);
	void PrintNVM(std::shared_ptr<IATCommandInterface> theATInterface);

private:
	std::vector<uint8_t> GetHexStringAsBytes(std::string hexString);
	uint8_t GetHexAsByte_Single(std::string hexString);
	std::string GetByteStringAsHexString(std::vector<uint8_t> theBytes);
	void PrintNVM(std::shared_ptr<nvmdata> dataToPrint);
	
	std::shared_ptr<SerialAndBluetooth> m_sandb;
};