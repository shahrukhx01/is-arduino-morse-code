/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Button
*/

// constants won't change. They're used here to set pin numbers:
const int buttonPin = D5;     // the number of the pushbutton pin
const int ledPin =  D0;      // the number of the LED pin
const int shortTime = 300;
const int longTime = 900;
// variables will change:
int buttonState = 1;         // variable for reading the pushbutton status
const String message = "Shahrukh3"; // message to be encoded in morse code 
    
void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  // begin serial port for debugging
  Serial.begin(9600);
}

void loop() {
  // read the state of the pushbutton value:
 buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    morseCode(message);
    delay(1000);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
}

void morseCode(String message){
message.toLowerCase();
// loop through the input message
for(auto x : message)
  {
     encodeMessage(x);
  }
}

void encodeMessage(char symbol){
Serial.println(symbol);
// map each input symbol to morse code using LED
switch (symbol) {
  case 'a':
    // a = . _
    blinkLED(shortTime);
    blinkLED(longTime);
    delay(longTime);
    break;
  case 'b':
    // statements
    break;
  case 'c':
    // statements
    break;
  case 'd':
    // statements
    break;
  case 'e':
    // statements
    break;
  case 'f':
    // statements
    break;
  case 'g':
    // statements
    break;
  case 'h':
    // h = ....
    blinkLED(shortTime);
    blinkLED(shortTime);
    blinkLED(shortTime);
    blinkLED(shortTime);
    delay(longTime);
    break;
  case 'i':
    // statements
    break;
  case 'j':
    // statements
    break;
  case 'k':
    // k = _ . _
    blinkLED(longTime);
    blinkLED(shortTime);
    blinkLED(longTime);
    delay(longTime);
    break;
  case 'l':
    // statements
    break;
  case 'm':
    // statements
    break;
  case 'n':
    // statements
    break;
  case 'o':
    // o = _ _ _
    blinkLED(longTime);
    blinkLED(longTime);
    blinkLED(longTime);
    delay(longTime);
    break;
  case 'p':
    // statements
    break;
  case 'q':
    // statements
    break;
  case 'r':
    // r = . _ .
    blinkLED(shortTime);
    blinkLED(longTime);
    blinkLED(shortTime);
    delay(longTime);
    break;
  case 's':
    // s = ...
    blinkLED(shortTime);
    blinkLED(shortTime);
    blinkLED(shortTime);
    delay(longTime);
    break;
  case 't':
    // statements
    break;
  case 'u':
    // u = . . _
    blinkLED(shortTime);
    blinkLED(shortTime);
    blinkLED(longTime);
    delay(longTime);
    break;
  case 'v':
    // statements
    break;
  case 'w':
    // statements
    break;
  case 'x':
    // statements
    break;
  case 'y':
    // statements
    break;
  case 'z':
    // statements
    break;
  default:
    // statements
    Serial.println("invalid input character");
    break;
}
}

void blinkLED(int time){
   digitalWrite(ledPin, HIGH);
   delay(time);
   digitalWrite(ledPin, LOW);
   delay(shortTime);
    
}
