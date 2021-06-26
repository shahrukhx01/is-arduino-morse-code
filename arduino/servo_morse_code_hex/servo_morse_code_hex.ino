/*
REFERENCES: 
https://arduinogetstarted.com/tutorials/arduino-button-long-press-short-press // for long short presses
https://forum.arduino.cc/t/arduino-push-button-double-click-function/409353 // for detecting double clicks
*/

#include <Servo.h>
Servo servoMotor;  // create servo object to control a servo
#define SERVO_PIN D1 // pin for servo moto
#define PUSH_BUTTON_PIN D4 // pin for push down button
#define MAX_MESSAGE_CHARACTERS 1000


// Constants who values will always stay same during program execution
const int ANGLE_STEP = 11; // step size for  angle  for servo (between 1 and 180)
const int MIN_SERVO_ANGLE = 180; //DEFAULT MOTOR HEAD POSITION - since 0 to 155 will be reserved for 0 to F hex characters also, the minimum angle servo motor can assume,
const int MAX_SERVO_ANGLE = 180; // stores the maxmimum angle servo motor can assume
const int SHORT_DURATION = 500; // duration for short signal using push button will be 500 milliseconds
const int LONG_DURATION = 2000; // duration for long signal using push button will be anything from 500 to 2000 milliseconds

// Variables whose value will change with time 
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0; // records the time when button was pushed down
unsigned long releasedTime = 0; // records the time when button was released back
String morseMessage; // morse string as an array
unsigned long doubleClickTimeLimit = 0; // when the button is first time pushed, then we attach time if it's clicked within that we take that as double click
int pushNumber = 0; // to record number of pushes
String messageCharacters[MAX_MESSAGE_CHARACTERS]; // to hold split characters of morse code
int messageIndexReverse; // for iterating over morse characters conveniently.
int messageIndex = 0; // for holding indices of message characters
bool decoding = false; // to hold state when to decode


void setup() {
  Serial.begin(9600);          //  setup serial
  //servoMotor.attach(SERVO_PIN);  // attaches the servo on pin 3 to the servo object
  servoMotor.attach(SERVO_PIN, 430, 2400);
  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  // pinMode(LED_BUILTIN, OUTPUT); //pin for LED for actuating space char
  // digitalWrite(LED_BUILTIN, LOW);
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
      Serial.println("decoding ...");
      Serial.println(morseMessage);
      parseMessage(morseMessage);
      morseMessage = ""; //reset message string
      decoding = true;      
    }else if( pushNumber == 2 ){ // detects if the button was pressed twice
      morseMessage.concat(" "); // if double click then add space
      Serial.println(morseMessage);
    }
    pushNumber = 0;
  }
  // once the message has been parsed and decoding flag is on, we can do actuation.
  if(decoding && messageIndex >= 0){
     Serial.print("morse character :");
     Serial.print(messageCharacters[messageIndexReverse]);
     Serial.println("");
    decodeMorseCode(messageCharacters[messageIndexReverse]);
    messageIndex--;
    messageIndexReverse++;
    if (messageIndex < 0) {
      decoding = false;
      messageIndexReverse = 0; // reset message traversal index
    }
    
    }
  
  // save the the last state
  lastState = currentState; 
  
}

void parseMessage(String message){
  int index = 0;
  bool spaceDetected = false;
  String characterMessage = "";
  const int MESSAGE_LENGTH = message.length();
  memset(messageCharacters, 0, sizeof(messageCharacters)); // reset message array
  messageIndex = 0;
  while (index < MESSAGE_LENGTH)
    {        
        if (spaceDetected == true && message.charAt(index) == ' '){
          spaceDetected = false;
          index++; // only increment when not doing substring
          messageCharacters[messageIndex] = " ";
          messageIndex++;
          }
        else if(message.charAt(index) == ' ')
        {
            characterMessage = message.substring(0, index); // get the character morse code out of message
            characterMessage.trim(); // remove any whitespace
            message = message.substring(index+1,  message.length()); // keep rest of the message
            index = 0; // restart index since we have chunked the message into substring.
            spaceDetected = true;
            messageCharacters[messageIndex] = characterMessage;
            messageIndex++;
        }else{
          spaceDetected = false;
          index++; // only increment when not doing substring
          }
        
    } 
    // decode rest of message if remaining
    message.trim(); // remove any whitespace before checking remaining message
    if(message.length() > 0){
       messageCharacters[messageIndex] = message;
      }
  
}

void decodeMorseCode(String message){
/*
WE USE FOLLOWING MAPPING OF HEXADECIMAL CHARACTERS

 0 -> 0
 1 -> 1
 2 -> 2
 3 -> 3
 4 -> 4
 5 -> 5
 6 -> 6
 7 -> 7
 8 -> 8
 9 -> 9
 A -> 10
 B -> 11
 C -> 12
 D -> 13
 E -> 14
 F -> 15
 
*/
  
  if (message.equals("._")){ // corresponds to 'A'
    String hexCode[2] = { "4", "1"};
    actuateServo(hexCode);
  }else if (message.equals("_...")){ // corresponds to 'B'
    String hexCode[2] = { "4", "2"};
    actuateServo(hexCode);
  }else if (message.equals("_._.")){ // corresponds to 'C'
    String hexCode[2] = { "4", "3"};
    actuateServo(hexCode);
  }else if (message.equals("_..")){ // corresponds to 'D'
    String hexCode[2] = { "4", "4"};
    actuateServo(hexCode);
  }else if (message.equals(".")){ // corresponds to 'E'
    String hexCode[2] = { "4", "5"};
    actuateServo(hexCode);
  }else if (message.equals(".._.")){ // corresponds to 'F'
    String hexCode[2] = { "4", "6"};
    actuateServo(hexCode);
  }else if (message.equals("__.")){ // corresponds to 'G'
    String hexCode[2] = { "4", "7"};
    actuateServo(hexCode);
  }else if (message.equals("....")){ // corresponds to 'H'
    String hexCode[2] = { "4", "8"};
    actuateServo(hexCode);
  }else if (message.equals("..")){ // corresponds to 'I'
    String hexCode[2] = { "4", "9"};
    actuateServo(hexCode);
  }else if (message.equals(".___")){ // corresponds to 'J'
    String hexCode[2] = { "4", "10"};
    actuateServo(hexCode);
  }else if (message.equals("_._")){ // corresponds to 'K'
    String hexCode[2] = { "4", "11"};
    actuateServo(hexCode);
  }else if (message.equals("._..")){ // corresponds to 'L'
    String hexCode[2] = { "4", "12"};
    actuateServo(hexCode);
  }else if (message.equals("__")){ // corresponds to 'M'
    String hexCode[2] = { "4", "13"};
    actuateServo(hexCode);
  }else if (message.equals("_.")){ // corresponds to 'N'
    String hexCode[2] = { "4", "14"};
    actuateServo(hexCode);
  }else if (message.equals("___")){ // corresponds to 'O'
    String hexCode[2] = { "4", "15"};
    actuateServo(hexCode);
  }else if (message.equals(".__.")){ // corresponds to 'P'
    String hexCode[2] = { "5", "0"};
    actuateServo(hexCode);
  }else if (message.equals("__._")){ // corresponds to 'Q'
    String hexCode[2] = { "5", "1"};
    actuateServo(hexCode);
  }else if (message.equals("._.")){ // corresponds to 'R'
    String hexCode[2] = { "5", "2"};
    actuateServo(hexCode);
  }else if (message.equals("...")){ // corresponds to 'S'
    String hexCode[2] = { "5", "3"};
    actuateServo(hexCode);
  }else if (message.equals("_")){ // corresponds to 'T'
    String hexCode[2] = { "5", "4"};
    actuateServo(hexCode);
  }else if (message.equals(".._")){ // corresponds to 'U'
    String hexCode[2] = { "5", "5"};
    actuateServo(hexCode);
  }else if (message.equals("..._")){ // corresponds to 'V'
    String hexCode[2] = { "5", "6"};
    actuateServo(hexCode);
  }else if (message.equals(".__")){ // corresponds to 'W'
    String hexCode[2] = { "5", "7"};
    actuateServo(hexCode);
  }else if (message.equals("_.._")){ // corresponds to 'X'
    String hexCode[2] = { "5", "8"};
    actuateServo(hexCode);
  }else if (message.equals("_.__")){ // corresponds to 'Y'
    String hexCode[2] = { "5", "9"};
    actuateServo(hexCode);
  }else if (message.equals("__..")){ // corresponds to 'Z'
    String hexCode[2] = { "5", "10"};
    actuateServo(hexCode);
  }else if (message.equals("_____")){ // corresponds to '0'
    String hexCode[2] = { "3", "0"};
    actuateServo(hexCode);
  }else if (message.equals(".____")){ // corresponds to '1'
    String hexCode[2] = { "3", "1"};
    actuateServo(hexCode);
  }else if (message.equals("..___")){ // corresponds to '2'
    String hexCode[2] = { "3", "2"};
    actuateServo(hexCode);
  }else if (message.equals("..,__")){ // corresponds to '3'
    String hexCode[2] = { "3", "3"};
    actuateServo(hexCode);
  }else if (message.equals("...._")){ // corresponds to '4'
    String hexCode[2] = { "3", "4"};
    actuateServo(hexCode);
  }else if (message.equals(".....")){ // corresponds to '5'
    String hexCode[2] = { "3", "5"};
    actuateServo(hexCode);
  }else if (message.equals("_....")){ // corresponds to '6'
    String hexCode[2] = { "3", "6"};
    actuateServo(hexCode);
  }else if (message.equals("__...")){ // corresponds to '7'
    String hexCode[2] = { "3", "7"};
    actuateServo(hexCode);
  }else if (message.equals("___..")){ // corresponds to '8'
    String hexCode[2] = { "3", "8"};
    actuateServo(hexCode);
  }else if (message.equals("____.")){ // corresponds to '9'
    String hexCode[2] = { "3", "9"};
    actuateServo(hexCode);
  }
  else if (message.equals(" ")){
    // move motor for space
    String hexCode[2] = { "2", "0"};
    actuateServo(hexCode);
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
  delay(2000); // stay here for 2 seconds
 }
