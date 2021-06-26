/*
REFERENCES: 
https://arduinogetstarted.com/tutorials/arduino-button-long-press-short-press // for long short presses
https://forum.arduino.cc/t/arduino-push-button-double-click-function/409353 // for detecting double clicks
*/

#include <Servo.h>
Servo servoMotor;  // create servo object to control a servo
#define SERVO_PIN D1 // pin for servo moto
#define PUSH_BUTTON_PIN D4 // pin for push down button

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
unsigned long doubleClickTimeLimit = 0; // when the button is first time pushed, then we attach time if it's clicked within that we take that as double click
int pushNumber = 0; // to record number of pushes

void setup() {
  Serial.begin(9600);          //  setup serial
  //servoMotor.attach(SERVO_PIN);  // attaches the servo on pin 3 to the servo object
  servoMotor.attach(SERVO_PIN, 430, 2400);
  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Servo Button ");
  servoMotor.write(MIN_SERVO_ANGLE); //initial position
}


/*
TODO:
2. Decode message using a loop and blink LED on space
*/


void loop() {
  // read the state of the switch/button:
  currentState = digitalRead(PUSH_BUTTON_PIN);

  if(lastState == HIGH && currentState == LOW) {       // button is pressed
    if (pushNumber == 0) {
       pressedTime = millis();
       doubleClickTimeLimit = pressedTime + SHORT_DURATION;  // if next push occurs in < than doubleClickTimeLimit then we take it as double click
       pushNumber = 1;
    }

   else if (pushNumber == 1 && millis() < doubleClickTimeLimit){ // button pressed within time next time considered as double click
      pushNumber = 2;
      }
  }
  else if(lastState == LOW && currentState == HIGH && pressedTime > 0 && pushNumber == 1){ // if button was clicked once and released so we record timestamp it was released at to determine b/w long/short press
    releasedTime = millis();
    } 
  else if(pushNumber > 0 && currentState == HIGH && millis() > doubleClickTimeLimit) { // button is released
    long pressDuration = releasedTime - pressedTime;

    if( pressDuration < SHORT_DURATION && pressDuration > 0 && pushNumber == 1 ){ // detects if the button press was short
      morseMessage.concat(".");
      Serial.println(morseMessage);
    }
    else if( pressDuration > SHORT_DURATION && pressDuration < LONG_DURATION && pushNumber == 1 ){ // detects if the button press was long
      morseMessage.concat("_");
      Serial.println(morseMessage);
    }
    else if( pressDuration > LONG_DURATION && pushNumber == 1 ){ // detects if the button press was too long
      String finalMessage = morseMessage; 
      Serial.println("decoding ...");
      Serial.println(finalMessage);
      //decodeMorseCode(finalMessage);
    }else if( pushNumber == 2 ){ // detects if the button was pressed twice
      morseMessage.concat(" "); // if double click then add space
      Serial.println(morseMessage);
    }
    pushNumber = 0;
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
