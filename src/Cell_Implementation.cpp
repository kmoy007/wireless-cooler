#include <Particle.h>
#include "Cell_Implementation.h"

// Various settings for how long to wait before rebooting and so forth
const unsigned long LISTEN_WAIT_FOR_REBOOT = 30000; // milliseconds, 0 = don't reboot on entering listening mode
const unsigned long CLOUD_WAIT_FOR_REBOOT = 180000; // milliseconds, 0 = don't reboot
const unsigned long PUBLISH_RATE_LIMIT = 1010; // milliseconds; to avoid sending events too rapidly
const unsigned long PING_TIMEOUT = 30000; // milliseconds
//const unsigned long APP_WATCHDOG_TIMEOUT = 60000; // milliseconds
const uint8_t NUM_REBOOTS_BEFORE_RESETTING_MODEM = 2; //
const size_t MAX_TESTERFN_ARGS = 5; // Used to split out arguments to testerFn Particle.function
const int SLEEP_TEST_PIN = D2; // Used for testing sleep with pin modes

// This code is used to track connection events, used mainly for debugging
// and making sure this code works properly.
const size_t CONNECTION_EVENT_MAX = 32; // Maximum number of events to queue, 16 bytes each
const unsigned long CONNECTION_EVENT_MAGIC = 0x5c39d415;

// This is the event that's used to publish data to your event log. It's sent in PRIVATE mode.
const char *CONNECTION_EVENT_NAME = "connEventStats";
// These are the defined event codes. Instead of a string, they're sent as an integer to make
// the output more compact, saving retained memory and allowing more events to fit in a Particle.publish/

/*
// Data for each event is stored in this structure
typedef struct { // 16 bytes per event
	unsigned long tsDate;
	unsigned long tsMillis;
	int eventCode;
	int data;
} ConnectionEventInfo;

// This structure is what's stored in retained memory
typedef struct {
	unsigned long eventMagic; // CONNECTION_EVENT_MAGIC
	uint8_t numFailureReboots;
	uint8_t reserved3;
	uint8_t reserved2;
	uint8_t reserved1;
	size_t eventCount;
	ConnectionEventInfo events[CONNECTION_EVENT_MAX];
} ConnectionEventData;

// This is where the retained memory is allocated. Currently 522 bytes.
// There are checks in connectionEventSetup() to initialize it on first
// use and if the format changes dramatically.
retained ConnectionEventData connectionEventData;

*/
Cell_Implementation::Cell_Implementation() :
	m_IsCellularReady(false),
	m_IsCloudConnected(false),
	m_ListeningStart(0),
	m_CloudCheckStart(0),
	m_connectionEventLastSent(0)
{

}

void Cell_Implementation::ResetModem()
{
	// We try not to do this because it requires the cellular handshake process to repeat,
	// which takes time and data.
	// This makes the reset more like unplugging the battery and power and doing a cold restart.

	Serial.println("resetting modem");

	Particle.disconnect();
		// 16:MT silent reset (with detach from network and saving of NVM parameters), with reset of the SIM card
	Cellular.command(30000, "AT+CFUN=16\r\n");
	Cellular.off();
	delay(1000);
}

bool Cell_Implementation::CloudConnectDebug() 
{
	int res = Cellular.command(PING_TIMEOUT, "AT+UPING=8.8.8.8\r\n");
	if (res != RESP_OK)
	{
		return false;
	}
	res = Cellular.command(PING_TIMEOUT, "AT+UPING=api.particle.io\r\n");
	
	return (res != RESP_OK);
}


void Cell_Implementation::DebugCellConnection_RebootIfYouThinkItsAGoodIdea() {

	// Check cellular status - used for event logging mostly
	bool temp = Cellular.ready();
	if (temp != m_IsCellularReady) {
		// Cellular state changed
		m_IsCellularReady = temp;
		Serial.printlnf("cellular %s", m_IsCellularReady ? "up" : "down");
	}

	// Check cloud connection status
	temp = Particle.connected();
	if (temp != m_IsCloudConnected) {
		// Cloud connection state changed
		m_IsCloudConnected = temp;
		Serial.printlnf("cloud connection %s", m_IsCloudConnected ? "up" : "down");

		if (!m_IsCloudConnected) 
		{
			// Cloud just disconnected, start measuring how long we've been disconnected
			m_CloudCheckStart = millis();
		}
	}

	if (!m_IsCloudConnected) {
		// Not connected to the cloud - check to see if we've spent long enough in this state to reboot
		if (CLOUD_WAIT_FOR_REBOOT != 0 && millis() - m_CloudCheckStart >= CLOUD_WAIT_FOR_REBOOT) {
			// The time to wait to connect to the cloud has expired, reboot

			bool forceResetModem = false;

			if (m_IsCellularReady) {
				// Generate events about the state of the connection before rebooting. We also
				// do some ping tests, and if we can't successfully ping, we force reset the mode
				// in hopes to clear this problem more quickly.
				forceResetModem = CloudConnectDebug();
				if (forceResetModem)
				{
					ResetModem();	
				}
			}
			System.reset();
		}
	}


	if (Cellular.listening()) {
		// Entered listening mode (blinking blue). Could be from holding down the MODE button, or
		// by repeated connection failures, see: https://github.com/spark/firmware/issues/687
		if (m_ListeningStart == 0) {
			m_ListeningStart = millis();
			Serial.println("entered listening mode");
		}
		else {
			if (LISTEN_WAIT_FOR_REBOOT != 0 && millis() - m_ListeningStart >= LISTEN_WAIT_FOR_REBOOT) {
				// Reboot
				ResetModem();
				System.reset();
			}
		}
	}
}





