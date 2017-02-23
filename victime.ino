#include "Arduino.h"


const int   sensor    = A0;     //pin sensing the light
bool    armed   = false;  //internal state of the readiness of the bomb, starts out disarmed
bool    exploding = false;  //internal state of the behavior of the bomb, starts out not exploding (duh!)

const int   armedLED  = 12;   //pin that controls the ARMED light indicator
const int   armButton = 2;    //pin sensing the ARM button pressed state
bool    btnState  = false;  //used by a debounce to store last button state;

const long refractory  = (1 * 60 * 1000L);//Minimal time between two explosions
unsigned long lastBoom= 0;    //store timestamp of last explosion, to compare

const int critLight = 750;    //The threshold for the light sensor above which it goes BOOM!
const int alarmPin  = 3;      //The pin that feeds power to the alarm!

/**
 * Returns true if a keydown event on the given digital pin has just happened, false for all other cases (keyup or hold down).
 * Also keeps a state variable in sync with the sensor :)
 */
bool debouncedAndPressed(int pin, bool& state, int wait){

  if(digitalRead(pin)){
    //check again to record final state
    delay(wait);
    if(digitalRead(pin)){
      //button is definitely in the pressed state, was it a change?
      if(!state){
        //it used to be low! this is a keydown event
        state = true;
        return true;
      }
      else{
        //it was already on, this is no keydown event <3 nunue 2017 love 4eva
        state = true;
        return false;
      }

    }
    else{
      //we just switched from hi to lo, this is a keyup event
      state = false;
      return false;
    }
  }
  else{
    //we are low anyway, and we know we are not within a transition
    state = false;
    return false;
  }
}


bool explode(){

  if( lastBoom == 0 ||(unsigned long)(millis() - lastBoom) > refractory){
    Serial.println("BOOM");
    //we are allowed to explode!
    lastBoom = millis();
    exploding = true;
    //analogWrite(alarmPin, 128);
    return true;
  }
  else{
    return false; //jaime full nunue :)
  }

}

void setup()
{
  pinMode(armedLED, OUTPUT);
  pinMode(alarmPin, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(9600);
}

// The loop function is called in an endless loop
void loop()
{
  //listen for arm button press
  if (debouncedAndPressed(armButton, btnState, 50)){
    armed = ! armed;
  }
  //sync the armed LED with the internal state of the bomb
  digitalWrite(armedLED, armed? HIGH : LOW);

  if(exploding && (millis() - lastBoom > refractory)){
	  //bomb has been exploding for too long,  stop it
	  exploding = false;
  }




  if(armed){
    //take a read from the light sensor
    if( !exploding && analogRead(sensor) > critLight){
      //we must go BOOM!
      Serial.println("boom!");
      explode();
    }
    else if(exploding){
    	analogWrite(alarmPin, HIGH);
    }
    else{
    	analogWrite(alarmPin, LOW);
    }
  }
  else{
    if(exploding){
      exploding = false;
      Serial.println("CANCELLING");
    }
  }
  Serial.println(exploding);
}
