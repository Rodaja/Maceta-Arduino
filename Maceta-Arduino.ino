//Needed libraries for the conections
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h >
#include <WiFiManager.h>
#include <ArduinoJson.h>

//Needed libraries for the air humidity and temperature sensor
#include <DHT.h>
#include <DHT_U.h>

//Defining sensor pins
#define GROUNDSENSOR A0
#define DHTPIN 12

// Defining the air sensor type
#define DHTTYPE DHT11

//Constants
String URL = "http://161.35.69.225:8080/gardenia/api/flowerpots";
String MAC = WiFi.macAddress();

int maxGroundHumidity = 270; //To calibrate
int minGroundHumidity = 706; //To calibrate

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200); //To debug
  dht.begin();
  setWifiManager();
  sendMacAddress();

}

void loop() {
  int groundHumidity = getDataGroundSensor();
  int airHumidity = getDataAirHumidity();
  int airTemperature = getDataAirTemperature();
  sendPlotData(groundHumidity, airHumidity, airTemperature);
  delay(5000);

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
  int analogGroundSensorValue = analogRead(GROUNDSENSOR);
  
  // We use map() here to adjust the values to adjust the read values to the percentages we want to use
  //sensor reading, min value, max value , max percentage, min percentage
  int valueGroundSensor = map(analogGroundSensorValue, maxGroundHumidity, minGroundHumidity, 100, 0);   
  
  Serial.print(valueGroundSensor); // To debug
  Serial.println("%");

  return valueGroundSensor;
}

int getDataAirHumidity(){
  int humidity = dht.readHumidity();
  return humidity;
}

int getDataAirTemperature(){
  int temperature = dht.readTemperature();
  return temperature;
}

void sendPlotData(int groundHumidity, int airHumidity, int airTemperature) {
  HTTPClient http;    //Declare object of class HTTPClient

  //We indicates the destination
  http.begin(URL + "/" + MAC);

  //We have to specify de format of the content we are going to send in this case JSON
  http.addHeader("Content-Type", "application/json");

  //We form here the JSON structure
  StaticJsonBuffer<200> jsonBuffer;
  char json[256];
  JsonObject& root = jsonBuffer.createObject();
  root["macAddress"] = MAC;
  root["groundHumidity"] = groundHumidity;
  root["airHumidity"] = airHumidity;
  root["airTemperature"] = airTemperature;
  root.printTo(json, sizeof(json));

  // We send the request here and receive a code status
  int httpCode = http.PUT(json);

  //Here we recieve the payload status
  String payload = http.getString();

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload

  http.end();
}
