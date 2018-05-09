#include "Adafruit_BluefruitLE_SPI.h"


class ScopedBLEModeSetter
{
public:
	ScopedBLEModeSetter(Adafruit_BluefruitLE_SPI* ble, uint8_t new_mode) :
		m_BLE(ble)
	{
		if (m_BLE == NULL)
		{
			Serial.println("ERROR.  ScopedBLEModeSetter constructor (m_BLE == NULL)!!");
			delay(1000); //let it flush
		}
		else
		{
			m_OriginalMode = m_BLE->getMode();

			if ( new_mode != m_OriginalMode ) 
			{				
				m_BLE->setMode(new_mode);
			}
		}	
	}
	virtual ~ScopedBLEModeSetter()
	{
		m_BLE->setMode(m_OriginalMode);
	}

private:
	Adafruit_BluefruitLE_SPI* m_BLE;
	uint8_t m_OriginalMode;
};