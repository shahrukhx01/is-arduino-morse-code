#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
void setup()
{
  Serial.begin(115200);
  WiFi.begin("iPhone", "pass1234");

  Serial.print("Connecting");
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");   
  }

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    WiFiClient client; //Declare an object of class WiFiClient
    HTTPClient http;  //Declare an object of class HTTPClient
    http.begin(client, "http://129.159.252.59");  //Specify request destination
    int httpCode = http.GET();                                  //Send the request
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);             //Print the response payload
    }
    http.end();   //Close connection
  }
  delay(30000);    //Send a request every 30 seconds
  }
