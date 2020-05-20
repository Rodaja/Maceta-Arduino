#include <ESP8266httpUpdate.h>

#define __DEBUG__

//Needed libraries for the conections
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h >
#include <WiFiManager.h>
#include <ArduinoOTA.h>

void setup() {
  Serial.begin(115200); //To debug
  setWifiManager();
}

void loop() {
  updateSystem();
  delay(5000);
}

void setWifiManager() {
  WiFiManager wifiManager;

  wifiManager.autoConnect("Gardenia");
}

void updateSystem(){

    t_httpUpdate_return ret =ESPhttpUpdate.update("161.35.69.225", 8080, "/gardenia/api/flowerpots/update");
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
