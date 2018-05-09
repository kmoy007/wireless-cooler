#pragma once

enum ConnectionEventCode {
	CONNECTION_EVENT_SETUP_STARTED = 0,		// 0
	CONNECTION_EVENT_CELLULAR_READY,		// 1
	CONNECTION_EVENT_CLOUD_CONNECTED,		// 2
	CONNECTION_EVENT_LISTENING_ENTERED,		// 3
	CONNECTION_EVENT_MODEM_RESET,			// 4
	CONNECTION_EVENT_REBOOT_LISTENING,		// 5
	CONNECTION_EVENT_REBOOT_NO_CLOUD,		// 6
	CONNECTION_EVENT_PING_DNS,				// 7
	CONNECTION_EVENT_PING_API,				// 8
	CONNECTION_EVENT_APP_WATCHDOG,			// 9
	CONNECTION_EVENT_TESTERFN_REBOOT,		// 10
	CONNECTION_EVENT_TESTERFN_APP_WATCHDOG,	// 11
	CONNECTION_EVENT_SLEEP					// 12
};
extern void ConnectionEventAdd(int eventCode, int data /* = 0 */);


class Cell_Implementation
{
public:
	Cell_Implementation();
	// This is used to slow down publishing of data to once every 1010 milliseconds to avoid
	// exceeding the publish rate limit.
	

	// This should be called during setup()
	void ConnectionEventSetup(); 

	// This should be called from loop()
	// If there are queued events and there is a cloud connection they're published, oldest first.
	void PublishEvents();
	void DebugCellConnection_RebootIfYouThinkItsAGoodIdea();



	void ResetModem();

private:
	
	// This is called when timing out connecting to the cloud. It adds some debugging events to
	// help log the current state for debugging purposes.
	// It returns true to force a modem reset immediately, false to use the normal logic for whether to reset the modem.
	bool CloudConnectDebug();

	// reason is the reason code, one of the CONNECTION_EVENT_* constants
	// forceResetMode will reset the modem even immediately instead of waiting for multiple failures
	void SmartReboot(int reason, bool forceResetModem);
	




//	void testerFnLoopHandler(char *mutableData); // forward declaration


	unsigned long m_connectionEventLastSent;
	bool m_IsCellularReady;
	bool m_IsCloudConnected;
	unsigned long m_ListeningStart;
	unsigned long m_CloudCheckStart;



};

/*

// This does the actual work from the Particle.function(). It's called from looo().
void testerFnLoopHandler(char *mutableData) {
	// Parse argument into space-separated fields
	const char *argv[MAX_TESTERFN_ARGS];
	size_t argc = 0;

	char *cp = strtok(mutableData, " ");
	while(cp && argc < MAX_TESTERFN_ARGS) {
		argv[argc++] = cp;
		cp = strtok(NULL, " ");
	}
	if (argc == 0) {
		// No parameter, nothing to do
		return;
	}

	// Delay a bit here to make sure the function result is returned, otherwise if we
	// immediately go to sleep the function may return a timeout error.
	delay(500);

	// Process options here
	if (strcmp(argv[0], "reset") == 0) {
		smartReboot(CONNECTION_EVENT_TESTERFN_REBOOT, false);
	}
	else
	if (strcmp(argv[0], "modemReset") == 0) {
		smartReboot(CONNECTION_EVENT_TESTERFN_REBOOT, true);
	}
	else
	if (strcmp(argv[0], "appWatchdog") == 0) {
		connectionEventAdd(CONNECTION_EVENT_TESTERFN_APP_WATCHDOG, APP_WATCHDOG_TIMEOUT);

		while(true) {
			// Infinite loop!
		}
	}
	else
	if (strcmp(argv[0], "sleep") == 0 && argc >= 2) {
		// example usage from the Particle CLI:
		// particle call electron2 testerFn "sleep networkStandby 15"

		// optional duration in seconds, if not specified the default is 30
		int duration = 30;
		if (argc >= 3) {
			duration = atoi(argv[2]);
			if (duration == 0) {
				duration = 30;
			}
		}

		connectionEventAdd(CONNECTION_EVENT_SLEEP);

		if (strcmp(argv[1], "deep") == 0) {
			// SLEEP_MODE_DEEP requires cellular handshaking again (blinking green) and also
			// restarts running setup() again, so you'll get an event 0 (CONNECTION_EVENT_SETUP_STARTED)
			System.sleep(SLEEP_MODE_DEEP, duration);
		}
		else
		if (strcmp(argv[1], "stop") == 0) {
			// stop mode sleep stops the processor but execution will continue in loop() without going through setup again
			// This mode will shut down the cellular modem to save power so upon wake it requires cellular handshaking
			// again (blinking green)
			System.sleep(SLEEP_TEST_PIN, FALLING, duration);
		}
		else
		if (strcmp(argv[1], "networkStandby") == 0) {
			// stop mode sleep stops the processor but execution will continue in loop() without going through setup again
			// This mode keeps the cellular modem alive, so you should go right back into blinking cyan to handshake
			// to the cloud only
			System.sleep(SLEEP_TEST_PIN, FALLING, duration, SLEEP_NETWORK_STANDBY);
		}
	}
}
*/

