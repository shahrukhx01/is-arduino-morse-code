/*
REFERENCES: 
https://medium.com/@yifeiyin/communication-between-arduino-and-unity-9fdcccc2be3f
*/


// constants
const int BUTTON_PIN = D4; // set push down button pin


void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN, INPUT_PULLUP); // sets button mode
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW){
    Serial.flush(); // wait for the serial communication to complete
    Serial.println(1); // write 1 to the serial port
    }
    else{
      Serial.flush(); // wait for the serial communication to complete
      Serial.println(0);  // write 0 to the serial port
    }
   delay(50);
}
