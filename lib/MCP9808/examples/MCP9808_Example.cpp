/*
Spark Core MCP9808 Temperature Sensor Library
By: Romain MP
Licence: GPL v3
*/

#include "MCP9808.h"

MCP9808 mcp = MCP9808();

void setup()
{
	Serial.begin(9600);
	
	pinMode(C0, OUTPUT);
  	digitalWrite(C0, LOW);

	pinMode(C1, OUTPUT);
  	digitalWrite(C1, LOW);

	pinMode(C2, OUTPUT);
  	digitalWrite(C2, LOW);

	pinMode(C3, INPUT);  //ALERT PIN
  	
	//pinMode(C4, OUTPUT);
  	//digitalWrite(, LOW);

	//pinMode(C5, OUTPUT);
  	//digitalWrite(, LOW);

	pinMode(D0, OUTPUT);  //GND
  	digitalWrite(D0, LOW);

	pinMode(D1, OUTPUT); //VCC
  	digitalWrite(D1, LOW);

  	delay(100);

	pinMode(D1, OUTPUT); //TURN IT ON
  	digitalWrite(D1, HIGH);

	
    delay(5000);
    
	Serial.println("MCP9808 test");

	// Wait for the sensor to come up
	while(! mcp.begin()){
	    Serial.println("MCP9808 not found");
	    delay(500);
	}

	// Set resolution to the maximum (slowest conversion)
	mcp.setResolution(MCP9808_SLOWEST);

	Serial.println("MCP9808 OK");
}

void loop()
{
	Serial.print("Temp:"); Serial.println(mcp.getTemperature(), 4);
	delay(250);
}

