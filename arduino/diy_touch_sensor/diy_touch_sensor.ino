/*
REFERENCES: 
https://forum.arduino.cc/t/capacitive-sensing-without-library/484848
*/
#include <Servo.h>
Servo servoMotor;  // create servo object to control a servo
#define SERVO_PIN D8 // pin for servo moto
#define INPUT_PIN D2 //pin D2 used for input
#define OUTPUT_PIN D1 // pin for output
#define MAX_MESSAGE_CHARACTERS 1000

// Constants who values will always stay same during program execution
const int ANGLE_STEP = 11; // step size for  angle  for servo (between 1 and 180)
const int MIN_SERVO_ANGLE = 180; //DEFAULT MOTOR HEAD POSITION - since 0 to 155 will be reserved for 0 to F hex characters also, the minimum angle servo motor can assume,
const int MAX_SERVO_ANGLE = 180; // stores the maxmimum angle servo motor can assume

// Variables whose value will change with time 
unsigned long startTime ; // to record start of loop
unsigned long measuredTime; // to record how much it took input pin to goto high, in absence of touch it will be 0 seconds
String morseMessage; // morse string
unsigned long nextTouchTimeLimit = 0;
int touchNumber = 0;
unsigned long pressedTime  = 0; // records the time when sensor was touched
String messageCharacters[MAX_MESSAGE_CHARACTERS]; // to hold split characters of morse code
int messageIndexReverse = 0; // for iterating over morse characters conveniently.
int messageIndex = 0; // for holding indices of message characters
bool decoding = false; // to hold state when to decode

void setup() {
  pinMode(INPUT_PIN, INPUT); //set short input pin
  pinMode(OUTPUT_PIN, OUTPUT); //set short output pin  
  Serial.begin(500000); //start serial communication
  servoMotor.attach(SERVO_PIN, 430, 2400);
  servoMotor.write(MIN_SERVO_ANGLE); //initial position
}

void loop() {
// read the state of the switch/button:
bool isTouch = senseCapacitiveTouch(INPUT_PIN, OUTPUT_PIN);
if(isTouch && touchNumber == 0) {       // touch is present
  pressedTime = millis();
  nextTouchTimeLimit = pressedTime + 1000;
  touchNumber = 1;
  Serial.println("touch 1");
  }
 else if(isTouch && millis() < nextTouchTimeLimit && touchNumber == 1){
  pressedTime = millis();
  nextTouchTimeLimit = pressedTime + 1000;
  touchNumber = 2;
  Serial.println("touch 2");
  }
 else if(isTouch && millis() < nextTouchTimeLimit && touchNumber == 2){
  touchNumber = 3;
  nextTouchTimeLimit = pressedTime + 1000;
  Serial.println("touch 3");
  }
  else if(isTouch && millis() < nextTouchTimeLimit && touchNumber == 3){
  Serial.println("touch 4");
  touchNumber = 4;
  parseMessage(morseMessage);
  morseMessage = ""; //reset message string
  decoding = true;      
  }
  else if (millis() > nextTouchTimeLimit && touchNumber >= 1){
    if(touchNumber == 1) morseMessage.concat(".");
    if(touchNumber == 2) morseMessage.concat("_");
    if(touchNumber == 3) morseMessage.concat(" ");
    Serial.println(morseMessage);
    touchNumber = 0;
    }
}

bool senseCapacitiveTouch(int input, int output){
  
  bool touch = false;
  digitalWrite(output,HIGH); //set output pin high

  startTime = micros();
  while( digitalRead(input) == 0){ // wait for the input pin to goto high
      ESP.wdtFeed(); // keep feeding watchdog to avoid board reset
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
 }
  measuredTime = micros() - startTime;
  if(measuredTime/1000 > 0){
    touch = true;
   delay(400);
    }


   digitalWrite(input, LOW); // reset input pin
   digitalWrite(output, LOW); // reset output pin  
   return touch;
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
