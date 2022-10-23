#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "Hello From ESP8266 !");
}

void handleNotFound() {
  server.send(404, "text/plain", "File Not Found");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin("bcs", "28911603");
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.println("Wifi connecting...");
      notConnectedCounter++;
      if(notConnectedCounter > 150) { // Reset board if not connected after 5s
          Serial.println("Resetting due to Wifi not connecting...");
          ESP.restart();
      }
  }
  Serial.print("Wifi connected, IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.begin();

  //ping esp.local
  if (MDNS.begin("esp")) {
    Serial.println("MDNS responder started");
  }

  //curl http://esp.local/
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
}