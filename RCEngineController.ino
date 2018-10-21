

// Thius code is based on code in Blog posts illustrating reading an RC Receiver with
// micro controller interrupts.
//
//
// http://rcarduino.blogspot.com/
//
// See also http://rcarduino.blogspot.co.uk/2012/04/how-to-read-multiple-rc-channels-draft.html

#define THROTTLE_SIGNAL_IN 1 // INTERRUPT 1 = DIGITAL PIN 3 - use the interrupt number in attachInterrupt
#define THROTTLE_SIGNAL_IN_PIN 3 // INTERRUPT 1 = DIGITAL PIN 3 - use the PIN number in digitalRead

#define NEUTRAL_THROTTLE 1500 // this is the duration in microseconds of neutral throttle on an electric RC Car

#include <Servo.h>

Servo throtservo; //this servo can control throttle butterflies
Servo motservo;   //this servo controls the belt drive of the engine.
volatile int nThrottleIn = NEUTRAL_THROTTLE; // volatile, we set this in the Interrupt and read it in loop so it must be declared volatile
volatile int nThrottleOut = NEUTRAL_THROTTLE;
volatile int nMotorOut = NEUTRAL_THROTTLE;
volatile unsigned long ulStartPeriod = 0; // set in the interrupt
volatile boolean bNewThrottleSignal = false; // set in the interrupt and read in the loop
// we could use nThrottleIn = 0 in loop instead of a separate variable, but using bNewThrottleSignal to indicate we have a new signal
// is clearer for this first example

void setup()
{
  // tell the Arduino we want the function calcInput to be called whenever INT1 (digital pin 3) changes from HIGH to LOW or LOW to HIGH
  // catching these changes will allow us to calculate how long the input pulse is
  attachInterrupt(THROTTLE_SIGNAL_IN,calcInput,CHANGE);
  throtservo.attach(9);
  motservo.attach(10);
  //Serial.begin(9600); //this code can be used print the throttle input to the serial console, since this sketch is being written with the Adafruit Pro Trinket as it's target, this is not functional.
}

void loop()
{
 // if a new throttle signal has been measured, lets print the value to serial, if not our code could carry on with some other processing
 if(bNewThrottleSignal)
 {

   //Serial.println(nThrottleIn);//this code can be used print the throttle input to the serial console, since this sketch is being written with the Adafruit Pro Trinket as it's target, this is not functional.
   nThrottleOut = abs(nThrottleIn - 1500) + 1500;
   throtservo.writeMicroseconds(nThrottleOut);
   nMotorOut = abs(nThrottleIn - 1500)/25 + 1500;
   motservo.writeMicroseconds(nMotorOut);
   // set this back to false when we have finished
   // with nThrottleIn, while true, calcInput will not update
   // nThrottleIn
   bNewThrottleSignal = false;
 }

 // other processing ...
}

void calcInput()
{
  // if the pin is high, its the start of an interrupt
  if(digitalRead(THROTTLE_SIGNAL_IN_PIN) == HIGH)
  {
    // get the time using micros - when our code gets really busy this will become inaccurate, but for the current application its
    // easy to understand and works very well
    ulStartPeriod = micros();
  }
  else
  {
    // if the pin is low, its the falling edge of the pulse so now we can calculate the pulse duration by subtracting the
    // start time ulStartPeriod from the current time returned by micros()
    if(ulStartPeriod && (bNewThrottleSignal == false))
    {
      nThrottleIn = (int)(micros() - ulStartPeriod);
      ulStartPeriod = 0;

      // tell loop we have a new signal on the throttle channel
      // we will not update nThrottleIn until loop sets
      // bNewThrottleSignal back to false
      bNewThrottleSignal = true;
    }
  }
}
