/*
REFERENCES: 
https://forum.arduino.cc/t/capacitive-sensing-without-library/484848
*/

#define LED_PIN D8 // pin for LED
#define INPUT_PIN D2 //pin D2 used for input
#define OUTPUT_PIN D1 // pin for output
#define MAX_MESSAGE_CHARACTERS 1000
#define BLINK_DURATION 500 // duration of blink to mark start of a new binary digit
#define BINARY_SIGNAL_DURATION 2000 // duration for actuating LED for a single BINARY digit


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
  Serial.begin(9600); //start serial communication
  pinMode(LED_PIN, OUTPUT); // set LED pins for out, D8 for representing binary 1s
  pinMode(LED_BUILTIN, OUTPUT); // set LED pins for out, LED_BUILTIN for representing binary 0s
  digitalWrite(LED_PIN, LOW);
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
  // using binary values for each character and actuate using LED
  if (message.equals("._")){ // corresponds to 'A'
    String binaryCode[8] = { "0","1","0","0","0","0","0","1"};
    actuateLED(binaryCode);
  }else if (message.equals("_...")){ // corresponds to 'B'
    
    String binaryCode[8] = { "0","1","0","0","0","0","1","0"};
    actuateLED(binaryCode);
  }else if (message.equals("_._.")){ // corresponds to 'C'
    String binaryCode[8] = { "0","1","0","0","0","0","1","1"};
    actuateLED(binaryCode);
  }else if (message.equals("_..")){ // corresponds to 'D'
    String binaryCode[8] = { "0","1","0","0","0","1","0","0"};
    actuateLED(binaryCode);
  }else if (message.equals(".")){ // corresponds to 'E'
    String binaryCode[8] = { "0","1","0","0","0","1","0","1"};
    actuateLED(binaryCode);
  }else if (message.equals(".._.")){ // corresponds to 'F'
    String binaryCode[8] = { "0","1","0","0","0","1","1","0"};
    actuateLED(binaryCode);
  }else if (message.equals("__.")){ // corresponds to 'G'
    String binaryCode[8] = {"0","1","0","0","0","1","1","1"};
    actuateLED(binaryCode);
  }else if (message.equals("....")){ // corresponds to 'H'
    String binaryCode[8] = {"0","1","0","0","1","0","0","0"};
    actuateLED(binaryCode);
  }else if (message.equals("..")){ // corresponds to 'I'
    String binaryCode[8] = {"0","1","0","0","1","0","0","1"};
    actuateLED(binaryCode);
  }else if (message.equals(".___")){ // corresponds to 'J'
    String binaryCode[8] = { "0","1","0","0","1","0","1","0"};
    actuateLED(binaryCode);
  }else if (message.equals("_._")){ // corresponds to 'K'
    String binaryCode[8] = {"0","1","0","0","1","0","1","1"};
    actuateLED(binaryCode);
  }else if (message.equals("._..")){ // corresponds to 'L'
    String binaryCode[8] = {"0","1","0","0","1","1","0","0"};
    actuateLED(binaryCode);
  }else if (message.equals("__")){ // corresponds to 'M'
    String binaryCode[8] = {"0","1","0","0","1","1","0","1"};
    actuateLED(binaryCode);
  }else if (message.equals("_.")){ // corresponds to 'N'
    String binaryCode[8] = {"0","1","0","0","1","1","1","0"};
    actuateLED(binaryCode);
  }else if (message.equals("___")){ // corresponds to 'O'
    String binaryCode[8] = {"0","1","0","0","1","1","1","1"};
    actuateLED(binaryCode);
  }else if (message.equals(".__.")){ // corresponds to 'P'
    String binaryCode[8] = {"0","1","0","1","0","0","0","0"};
    actuateLED(binaryCode);
  }else if (message.equals("__._")){ // corresponds to 'Q'
    String binaryCode[8] = {"0","1","0","1","0","0","0","1"};
    actuateLED(binaryCode);
  }else if (message.equals("._.")){ // corresponds to 'R'
    String binaryCode[8] = {"0","1","0","1","0","0","1","0"};
    actuateLED(binaryCode);
  }else if (message.equals("...")){ // corresponds to 'S'
    String binaryCode[8] = {"0","1","0","1","0","0","1","1"};
    actuateLED(binaryCode);
  }else if (message.equals("_")){ // corresponds to 'T'
    String binaryCode[8] = {"0","1","0","1","0","1","0","0"};
    actuateLED(binaryCode);
  }else if (message.equals(".._")){ // corresponds to 'U'
    String binaryCode[8] = {"0","1","0","1","0","1","0","1"};
    actuateLED(binaryCode);
  }else if (message.equals("..._")){ // corresponds to 'V'
    String binaryCode[8] = {"0","1","0","1","0","1","1","0"};
    actuateLED(binaryCode);
  }else if (message.equals(".__")){ // corresponds to 'W'
    String binaryCode[8] = {"0","1","0","1","0","1","1","1"};
    actuateLED(binaryCode);
  }else if (message.equals("_.._")){ // corresponds to 'X'
    String binaryCode[8] = {"0","1","0","1","1","0","0","0"};
    actuateLED(binaryCode);
  }else if (message.equals("_.__")){ // corresponds to 'Y'
    String binaryCode[8] = {"0","1","0","1","1","0","0","1"};
    actuateLED(binaryCode);
  }else if (message.equals("__..")){ // corresponds to 'Z'
    String binaryCode[8] = {"0","1","0","1","1","0","1","0"};
    actuateLED(binaryCode);
  }else if (message.equals("_____")){ // corresponds to '0'
    String binaryCode[8] = {"0","0","1","1","0","0","0","0"};
    actuateLED(binaryCode);
  }else if (message.equals(".____")){ // corresponds to '1'
    String binaryCode[8] = {"0","0","1","1","0","0","0","1"};
    actuateLED(binaryCode);
  }else if (message.equals("..___")){ // corresponds to '2'
    String binaryCode[8] = {"0","0","1","1","0","0","1","0"};
    actuateLED(binaryCode);
  }else if (message.equals("..,__")){ // corresponds to '3'
    String binaryCode[8] = {"0","0","1","1","0","0","1","1"};
    actuateLED(binaryCode);
  }else if (message.equals("...._")){ // corresponds to '4'
    String binaryCode[8] = {"0","0","1","1","0","1","0","0"};
    actuateLED(binaryCode);
  }else if (message.equals(".....")){ // corresponds to '5'
    String binaryCode[8] = {"0","0","1","1","0","1","0","1"};
    actuateLED(binaryCode);
  }else if (message.equals("_....")){ // corresponds to '6'
    String binaryCode[8] = {"0","0","1","1","0","1","1","0"};
    actuateLED(binaryCode);
  }else if (message.equals("__...")){ // corresponds to '7'
    String binaryCode[8] = {"0","0","1","1","0","1","1","1"};
    actuateLED(binaryCode);
  }else if (message.equals("___..")){ // corresponds to '8'
    String binaryCode[8] = {"0","0","1","1","1","0","0","0"};
    actuateLED(binaryCode);
  }else if (message.equals("____.")){ // corresponds to '9'
    String binaryCode[8] = {"0","0","1","1","1","0","0","1"};
    actuateLED(binaryCode);
  }
  else if (message.equals(" ")){
    // actuate LED for space
    String binaryCode[8] = {"0","0","1","0","0","0","0","0"};
    actuateLED(binaryCode);
    }
}
// fn to actuate LED for a sigle char a time when actuating for a string 
void actuateLED(String actuationBinaryCode[]){
  setLEDValue(-1, BLINK_DURATION); // blink LED to mark start of new character
  setLEDValue(actuationBinaryCode[0].toInt(), BINARY_SIGNAL_DURATION); // actuate LED for a single binary digit
  setLEDValue(-1, BLINK_DURATION); // blink LED to mark start of new character
  setLEDValue(actuationBinaryCode[1].toInt(), BINARY_SIGNAL_DURATION); // actuate LED for a single binary digit
  setLEDValue(-1, BLINK_DURATION); // blink LED to mark start of new character
  setLEDValue(actuationBinaryCode[2].toInt(), BINARY_SIGNAL_DURATION); // actuate LED for a single binary digit
  setLEDValue(-1, BLINK_DURATION); // blink LED to mark start of new character
  setLEDValue(actuationBinaryCode[3].toInt(), BINARY_SIGNAL_DURATION); // actuate LED for a single binary digit
  setLEDValue(-1, BLINK_DURATION); // blink LED to mark start of new character
  setLEDValue(actuationBinaryCode[4].toInt(), BINARY_SIGNAL_DURATION); // actuate LED for a single binary digit
  setLEDValue(-1, BLINK_DURATION); // blink LED to mark start of new character
  setLEDValue(actuationBinaryCode[5].toInt(), BINARY_SIGNAL_DURATION); // actuate LED for a single binary digit
  setLEDValue(-1, BLINK_DURATION); // blink LED to mark start of new character
  setLEDValue(actuationBinaryCode[6].toInt(), BINARY_SIGNAL_DURATION); // actuate LED for a single binary digit
  setLEDValue(-1, BLINK_DURATION); // blink LED to mark start of new character
  setLEDValue(actuationBinaryCode[7].toInt(), BINARY_SIGNAL_DURATION); // actuate LED for a single binary digit
 }

 
 // turns on or keeps LED off
void setLEDValue(int signal, int duration){
  if(signal == 1) {
    Serial.println(signal);
    digitalWrite(LED_PIN, HIGH);
    delay(duration);
    digitalWrite(LED_PIN, LOW);
  }else if(signal == 0){
    Serial.println(signal);
    delay(duration);
    }else if(signal == -1){
    digitalWrite(LED_PIN, LOW);
    delay(duration);
    digitalWrite(LED_PIN, HIGH);
    delay(duration);
    digitalWrite(LED_PIN, LOW);
      }
 } 
