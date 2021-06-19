
//constants
const int PIN_NUMBER = LED_BUILTIN;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(PIN_NUMBER, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

// if ASCII value 49  (i.e., ASCII value of "1") is received over 
// serial port then LIGHT UP LED else keep it off
if (Serial.read() == 49){
  Serial.flush(); // wait for the serial communication to complete
  digitalWrite(PIN_NUMBER, LOW);
  delay(1000); // key LED on for 1 second for demonstration
 } else{
  Serial.flush(); // wait for the serial communication to complete
  digitalWrite(PIN_NUMBER, HIGH);
  }

delay(50);
}
