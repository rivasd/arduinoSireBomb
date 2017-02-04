#include "Arduino.h"
//The setup function is called once at startup of the sketch

const int sensor = 0;
bool armed = false;
const int 	armedLED = 12;
const int	armButton = 2;

bool debounce(bool state){
	int firstState = digitalRead(armButton);
	if(firstState != state){
		//retest again
		delay(5);
		if(firstState == digitalRead(armButton)){
			return firstState;
		}
		else return state;
	}
	else{
		return false;
	}
}


void setup()
{
	pinMode(12, OUTPUT);
	Serial.begin(9600);
}

// The loop function is called in an endless loop
void loop()
{
	//listen for arm button press
	armed = debounce(armed);

	//sync the armed LED with the internal state of the bomb
	digitalWrite(armedLED, armed? HIGH : LOW);

}
