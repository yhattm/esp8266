#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
void setup() {
  Serial.begin(115200);
  /**
   * Connet to Wifi
   */
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
  /**
   * Enable OTA update
   */
  ArduinoOTA.setHostname("esp");
  ArduinoOTA.begin();
}
void loop() {
  // Check for over the air update request and (if present) flash it
  ArduinoOTA.handle();
}