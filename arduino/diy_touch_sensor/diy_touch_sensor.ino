/*
REFERENCES: 
https://forum.arduino.cc/t/capacitive-sensing-without-library/484848
*/


int input = D5; //pin D5 used for input
int output = D0; //pin D0 used for output
unsigned long startTime ; // to record start of loop
unsigned long measuredTime; // to record how much it took input pin to goto high, in absence of touch it will be 0 seconds


void setup() {
  pinMode(input,INPUT); //set input pin
  pinMode(output,OUTPUT); //set output pin
  Serial.begin(9600); //start serial communication
}

void loop() {

  digitalWrite(output,HIGH); //set output pin high

  startTime = micros();
  while( digitalRead(input) == 0){ // wait for the input pin to goto high
      ESP.wdtFeed(); // keep feeding watchdog to avoid board reset
    }
  measuredTime = micros() - startTime;
  if(measuredTime/1000 > 0){
    Serial.println(measuredTime/1000);
    delay(500);
    }

   digitalWrite(input, LOW); // reset input pin
   digitalWrite(output, LOW); // reset output pin

}
