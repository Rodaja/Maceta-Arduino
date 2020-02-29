//Needed libraries
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h >
#include <WiFiManager.h>
#include <ArduinoJson.h>

//Defining sensor pins
#define groundSensor A0

//Constants
String URL = "http://192.168.1.205:8888/api/flowerpot";
String MAC = WiFi.macAddress();

int maxGroundHumidity = 270; //To calibrate
int minGroundHumidity = 706; //To calibrate

void setup() {
  Serial.begin(115200); //To debug
  setWifiManager();
  sendMacAddress();

}

void loop() {
  // put your main code here, to run repeatedly:

}

void setWifiManager() {
  WiFiManager wifiManager;

  wifiManager.autoConnect();
}

void sendMacAddress() {
  HTTPClient http;    //Declare object of class HTTPClient

  //We indicates the destination
  http.begin(URL);

  //We have to specify de format of the content we are going to send in this case JSON
  http.addHeader("Content-Type", "application/json");

  //We form here the JSON structure
  StaticJsonBuffer<200> jsonBuffer;
  char json[256];
  JsonObject& root = jsonBuffer.createObject();
  root["macAddress"] = MAC;
  root.printTo(json, sizeof(json));

  // We send the request here and receive a code status
  int httpCode = http.POST(json);

  //Here we recieve the payload status
  String payload = http.getString();

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload

  http.end();
}

int getDataGroundSensor(){
  int analogGroundSensorValue = analogRead(groundSensor);
  
  // We use map() here to adjust the values to adjust the read values to the percentages we want to use
  //sensor reading, min value, max value , max percentage, min percentage
  int valueGroundSensor = map(analogGroundSensorValue, maxGroundHumidity, minGroundHumidity, 100, 0);   
  
  Serial.print(valueGroundSensor); // To debug
  Serial.println("%");

  return valueGroundSensor;
}
