/*
REFERENCES: 
https://arduinogetstarted.com/tutorials/arduino-button-long-press-short-press
*/

#include <Servo.h>
Servo servoMotor;  // create servo object to control a servo
#define SERVO_PIN D4 // pin for servo moto
#define PUSH_BUTTON_PIN D1 // pin for push down button

// Constants who values will always stay same during program execution
const int ANGLE_STEP = 11; // step size for  angle  for servo (beteen 1 and 179)
const int MIN_SERVO_ANGLE = 5; // stores the minimum angle servo motor can assume
const int MAX_SERVO_ANGLE = 180; // stores the maxmimum angle servo motor can assume
const int SHORT_DURATION = 500; // duration for short signal using push button will be 500 milliseconds
const int LONG_DURATION = 2000; // duration for long signal using push button will be anything from 500 to 1000 milliseconds


// Variables whose value will change with time 
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0; // records the time when button was pushed down
unsigned long releasedTime = 0; // records the time when button was released back
String morseMessage; // morse string as an array

void setup() {
  Serial.begin(9600);          //  setup serial
  servoMotor.attach(SERVO_PIN);  // attaches the servo on pin 3 to the servo object
  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Servo Button ");
  servoMotor.write(MIN_SERVO_ANGLE); //initial position
}


/*
TODO:
1. Add double click events and append space on each double click to `morseMessage`
2. Decode message using a loop and blink LED on space
*/


void loop() {
  // read the state of the switch/button:
  currentState = digitalRead(PUSH_BUTTON_PIN);

  if(lastState == HIGH && currentState == LOW) {       // button is pressed
    pressedTime = millis();
  }
  else if(lastState == LOW && currentState == HIGH && pressedTime > 0) { // button is released
    releasedTime = millis();
    long pressDuration = releasedTime - pressedTime;

    if( pressDuration < SHORT_DURATION && pressDuration > 0 ){ // detects if the button press was short
      morseMessage.concat(".");
      Serial.println(morseMessage);
    }
    else if( pressDuration > SHORT_DURATION && pressDuration < LONG_DURATION ){ // detects if the button press was long
      morseMessage.concat("_");
      Serial.println(morseMessage);
    }
    else if( pressDuration > LONG_DURATION ){ // detects if the button press was too long
      String finalMessage = morseMessage; //.substring(1,morseMessage.length());
      Serial.println(finalMessage);
      decodeMorseCode(finalMessage);
    }
  }
  
  // save the the last state
  lastState = currentState; 
  
}

void decodeMorseCode(String message){
  if (message.equals("._")){ // corresponds to 'a'
    String hexCode[2] = { "6", "1"};
    actuateServo(hexCode);
  }else if (message.equals("")){
    // call to blink built in LED
    }
}

void actuateServo(String actuationHexCode[]){
  Serial.println(actuationHexCode[0].toInt());
  servoMotor.write(actuationHexCode[0].toInt() * ANGLE_STEP); // goto first digit of HEX code
  delay(2000); // stay at the first digit of HEX code
  servoMotor.write(MIN_SERVO_ANGLE); // goto back to initial angle
  delay(2000); // stay at initial angle
  Serial.println(actuationHexCode[1].toInt());
  servoMotor.write(actuationHexCode[1].toInt() * ANGLE_STEP); // goto second digit of HEX code
  delay(2000); // stay at the second digit of HEX code
  servoMotor.write(MIN_SERVO_ANGLE); // goto back to initial angle
 }
