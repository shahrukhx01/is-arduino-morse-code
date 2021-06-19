
int input = D5; //pin 2 used for input
int output = D0; //pin 4 used for output

void setup() {
 pinMode(input,INPUT); //set input pin
  pinMode(output,OUTPUT); //set output pin
  Serial.begin(9600); //start serial communication
}
unsigned long start_time ;
int lastState = HIGH;  // the previous state from the input pin
int currentState;
int counter  = 0;
int times[1024];
unsigned long total = 0;
unsigned long measured_time;
void loop() {

 currentState = digitalRead(input);
  digitalWrite(output,HIGH); //set output pin high

   start_time = micros();
  while( digitalRead(input) == 0){
  ESP.wdtFeed();
    //delay(1);
    }
  
    measured_time = micros() - start_time;
    if(measured_time/1000 > 0){
    Serial.println(measured_time/1000);
    delay(500);
}

   digitalWrite(input, LOW);
   digitalWrite(output, LOW);

}
