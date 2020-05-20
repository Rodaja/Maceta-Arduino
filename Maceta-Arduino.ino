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
String VERSION = "0.4.0";

//Constants screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire, -1);

int maxGroundHumidity = 270; //To calibrate
int minGroundHumidity = 706; //To calibrate

DHT dht(DHTPIN, DHTTYPE);

//IMAGES
// 'Brandbook-17', 128x64px
const unsigned char icon [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0xe0, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0xf8, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0xfe, 0x00, 0x0c, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x7f, 0x00, 0x0c, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x3f, 0xc0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x1f, 0xe0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x1f, 0xf0, 0x38, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x08, 0x0f, 0xf8, 0x30, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x0f, 0xbc, 0x70, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x0f, 0x9e, 0x70, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0c, 0x0f, 0xce, 0xe1, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0e, 0x0f, 0xc7, 0xe3, 0xdf, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x06, 0x0f, 0xc7, 0xcf, 0x9f, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0x0f, 0xc3, 0xcf, 0x3b, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x87, 0x0f, 0xc3, 0xde, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x1d, 0xc3, 0xfc, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x3d, 0xc3, 0xf8, 0xf7, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x39, 0xc1, 0xf1, 0xe7, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6b, 0xf9, 0xc1, 0xe1, 0xce, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7b, 0xf3, 0x81, 0xc3, 0xce, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x35, 0xe7, 0x81, 0x87, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x35, 0xc7, 0x01, 0x8f, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x3b, 0xcf, 0x01, 0x9e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1b, 0x8e, 0x03, 0xbc, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x97, 0x9c, 0x03, 0xb8, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8f, 0x1c, 0x03, 0xf1, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcf, 0x38, 0x03, 0xf3, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0x38, 0x03, 0xe7, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xee, 0x38, 0x07, 0xcf, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x70, 0x07, 0x9f, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x70, 0x07, 0x3f, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x1e, 0x70, 0x0f, 0x7a, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x0e, 0x70, 0x0e, 0xf7, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0e, 0x70, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x8e, 0x70, 0x1f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x87, 0x70, 0x1f, 0xfe, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'Logotipo-13', 128x64px
const unsigned char logo [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0f, 0xef, 0xc3, 0xfe, 0x1f, 0x9c, 0x3f, 0xbe, 0x07, 0xf8, 0x3f, 0xfe, 0x00, 0x3f, 0x07, 0xf8, 
  0x1f, 0xff, 0xcf, 0xff, 0x1f, 0xfc, 0xff, 0xfe, 0x1f, 0xfc, 0x3f, 0xff, 0x00, 0x3f, 0x1f, 0xfe, 
  0x3f, 0xff, 0xdf, 0xff, 0x9f, 0xfd, 0xff, 0xfe, 0x3f, 0xfe, 0x3f, 0xff, 0x80, 0x3f, 0x3f, 0xfe, 
  0x7f, 0xff, 0xcf, 0xff, 0x9f, 0xfd, 0xff, 0xfe, 0x7f, 0xff, 0x3f, 0xff, 0x80, 0x3f, 0x3f, 0xff, 
  0xff, 0xff, 0xc7, 0x9f, 0x9f, 0xff, 0xff, 0xfe, 0x7f, 0x3f, 0x3f, 0xff, 0xc0, 0x3f, 0x0e, 0x7f, 
  0xfe, 0x1f, 0xc0, 0x0f, 0x9f, 0xff, 0xf8, 0xfe, 0xfc, 0x1f, 0xbf, 0x9f, 0xc0, 0x3f, 0x00, 0x3f, 
  0xfc, 0x0f, 0xc0, 0x0f, 0xdf, 0xc3, 0xf0, 0x7e, 0xfc, 0x0f, 0xbf, 0x0f, 0xc0, 0x3f, 0x00, 0x1f, 
  0xfc, 0x0f, 0xc7, 0xff, 0xdf, 0x87, 0xe0, 0x7e, 0xff, 0xff, 0xbf, 0x0f, 0xc0, 0x3f, 0x0f, 0xff, 
  0xfc, 0x0f, 0xcf, 0xff, 0xdf, 0x87, 0xe0, 0x7e, 0xff, 0xff, 0xbf, 0x0f, 0xc0, 0x3f, 0x3f, 0xff, 
  0xfc, 0x1f, 0xdf, 0xff, 0xdf, 0x87, 0xe0, 0x7e, 0xff, 0xff, 0xbf, 0x0f, 0xc0, 0x3f, 0x7f, 0xff, 
  0xfe, 0x3f, 0xff, 0x0f, 0xdf, 0x83, 0xf0, 0x7e, 0xfc, 0x00, 0x3f, 0x0f, 0xc0, 0x3f, 0x7e, 0x1f, 
  0x7f, 0xff, 0xff, 0x0f, 0xdf, 0x83, 0xfd, 0xfe, 0xfe, 0x0c, 0x3f, 0x0f, 0xcf, 0x3f, 0x7c, 0x1f, 
  0x7f, 0xff, 0xff, 0x9f, 0xdf, 0x83, 0xff, 0xfe, 0x7f, 0xfe, 0x3f, 0x0f, 0xdf, 0xbf, 0x7e, 0x7f, 
  0x3f, 0xff, 0xff, 0xff, 0xdf, 0x81, 0xff, 0xfe, 0x7f, 0xff, 0x3f, 0x0f, 0xdf, 0xbf, 0x7f, 0xff, 
  0x1f, 0xff, 0xdf, 0xff, 0xdf, 0x80, 0xff, 0xfe, 0x3f, 0xff, 0x3f, 0x0f, 0xdf, 0xbf, 0x7f, 0xff, 
  0x07, 0xef, 0xdf, 0xff, 0xdf, 0x80, 0x7f, 0xfe, 0x0f, 0xfe, 0x3f, 0x0f, 0xdf, 0xbf, 0x3f, 0xff, 
  0x00, 0x0f, 0xc7, 0xef, 0xdf, 0x80, 0x3f, 0x3e, 0x03, 0xf8, 0x3f, 0x0f, 0xcf, 0x3f, 0x1f, 0x9f, 
  0x18, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x3f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x7f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'wifi', 24x24px
const unsigned char icon_wifi [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x07, 0xff, 0xe0, 0x1f, 0x00, 0xf8, 0x3c, 
  0x00, 0x3c, 0x70, 0x00, 0x0e, 0xe0, 0x00, 0x07, 0xc1, 0xff, 0x83, 0x03, 0xe7, 0xc0, 0x0f, 0x00, 
  0xf0, 0x1c, 0x00, 0x38, 0x08, 0x00, 0x10, 0x00, 0x3c, 0x00, 0x00, 0xff, 0x00, 0x01, 0xc3, 0x80, 
  0x01, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x3c, 0x00, 0x00, 
  0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


//TEMPORAL
int count = 0; 

void setup() {
  Serial.begin(115200); //To debug
  dht.begin();
  setWifiManager();
  sendMacAddress();
  setupScreen();
  showIntro();

}

void loop() {
  int groundHumidity = getDataGroundSensor();
  int airHumidity = getDataAirHumidity();
  int airTemperature = getDataAirTemperature();
  sendPlotData(groundHumidity, airHumidity, airTemperature);
  showDataOnScreen("H.  " + String(groundHumidity) + "%");
  showDataOnScreen("T.  " + String(airHumidity) + "C");
  showDataOnScreen("HR. " + String(airTemperature) + "%");
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

void showIntro(){
  display.clearDisplay();
  display.drawBitmap(0,0, icon,128, 64, WHITE);
  display.display();
  delay(2500);

  display.clearDisplay();
  display.drawBitmap(0,0, logo,128, 64, WHITE);
  display.display();
  delay(2500);
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
  display.setTextColor(SSD1306_WHITE);
  
  display.clearDisplay();
  
  display.drawBitmap(0,0, icon_wifi,24, 24, WHITE);
  
  display.setTextSize(1);
  display.setCursor(97,10);
  display.print(VERSION);

  display.setTextSize(3);
  
  display.cp437(true);
  display.setCursor(0,41);
  display.print(data);
  display.display();
  delay(2000);
  
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
