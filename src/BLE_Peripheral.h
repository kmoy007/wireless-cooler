#pragma once
#include "IPeripheralDevice.h"

#include "Adafruit_BluefruitLE_SPI.h"
#include "SerialAndBluetooth.h"
#include <memory>
#include "mpack_interface.h"
#include "IATCommandInterface.h"


class BLE_Peripheral : public IATCommandInterface, public IPeripheralDevice, public IBLEInterface, public std::enable_shared_from_this<BLE_Peripheral>
{
public:
    BLE_Peripheral(std::shared_ptr<SerialAndBluetooth> sandb, std::string deviceName);

	virtual std::string GetName() { return "Bluefruit";  }
	virtual void PrintInfo();

	virtual bool ATCommand_GetResult(const std::string& command, std::vector<std::string>& returnStrings);
	bool ATCommand_PrintResult(const std::string& command);

	virtual void UART_OutputString(const std::string& outputString);
	virtual Adafruit_BluefruitLE_SPI* GetRawBLEPtr() { return &m_ble; }
	virtual bool IsConnectedToMaster() { return m_ble.isConnected(); }

protected:
    virtual bool doSetup();
    virtual bool doMainLoop();

private:
	int GetTXFifoBytes();
	int GetRXFifoBytes();
	bool SetBLEDeviceName(std::string newName);
	std::string GetBLEDeviceName();
	//void PrintStrings(const std::vector<std::string>& returnStrings);
	bool GetOutputFromBLEDevice(std::vector<std::string>& result);

	void FactoryReset();
	bool ConfigureWakeUpIRQ();

private:
  std::shared_ptr<SerialAndBluetooth> m_sandb;
  Adafruit_BluefruitLE_SPI m_ble;
  std::string m_DeviceName;
};
