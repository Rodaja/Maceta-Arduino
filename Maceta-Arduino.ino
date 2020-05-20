#define __DEBUG__

//Needed libraries for the conections
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

//Libraries for OLED screen
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Needed libraries for the air humidity and temperature sensor
#include <DHT.h>
#include <DHT_U.h>

//Defining sensor pins
#define GROUNDSENSOR A0
#define DHTPIN 12

// Defining the air sensor type
#define DHTTYPE DHT11

//Constants server
int PORT = 8080;
String IP = "161.35.69.225";
String URL = "http://" + IP + ":" + PORT + "/gardenia/api/flowerpots";
String URL_UPDATE = URL + "/update";
String MAC = WiFi.softAPmacAddress();
String VERSION = "0.2";

//Constants screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire, -1);

int maxGroundHumidity = 270; //To calibrate
int minGroundHumidity = 706; //To calibrate

DHT dht(DHTPIN, DHTTYPE);


//TEMPORAL
int count = 0; 

void setup() {
  Serial.begin(115200); //To debug
  dht.begin();
  setWifiManager();
  sendMacAddress();
  setupScreen();

}

void loop() {
  int groundHumidity = getDataGroundSensor();
  int airHumidity = getDataAirHumidity();
  int airTemperature = getDataAirTemperature();
  sendPlotData(groundHumidity, airHumidity, airTemperature);
  showDataOnScreen("H.  " + String(groundHumidity) + "%");
  showDataOnScreen("T.  " + String(airHumidity) + "C");
  showDataOnScreen("HR. " + String(airTemperature) + "%");
  showDataOnScreen("V:  " + VERSION);
  showDataOnScreen("C:  " + String(count));
  count += 1;

  if (count > 1000){
    updateSystem();
    count = 0;
  }
  //updateDataOnScreen(String(groundHumidity), String(airHumidity), String(airTemperature));
}

void setupScreen(){
  // Start screen at 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
#ifdef __DEBUG__
    Serial.println("No se encuentra la pantalla OLED");
#endif
    while (true);
  }

  display.clearDisplay();
}

void setWifiManager() {
  WiFiManager wifiManager;

  wifiManager.autoConnect("Gardenia");
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

void showDataOnScreen(String data){
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  
  display.setCursor(0,30);
  display.clearDisplay();
  display.print(data);
  display.display();
  delay(2000);
  
}

void updateDataOnScreen(String groundHumidity, String airHumidity, String airTemperature){
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  
  display.setCursor(0,15);
  display.clearDisplay();
  display.println("H.     " + groundHumidity + "%");
  display.println("T.     " + airTemperature + "C");
  display.println("HR.    " + airHumidity + "%");
  display.display();
  delay(2500);
  
}

void updateSystem(){

    //Change with server data
    t_httpUpdate_return ret =ESPhttpUpdate.update(IP, PORT, "/gardenia/api/flowerpots/update");
    // Or:
    Serial.println(ret);//t_httpUpdate_return ret = ESPhttpUpdate.update(client, "server", 80, "file.bin");
    

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        
        break;
    }
    ESP.restart();
}
void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}
