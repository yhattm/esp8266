#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

char Version[] = "7.1.14";

#define LED 2
#define SWITCH 0
ESP8266WebServer server(80);
uint8_t macAddr[6];

void handleRoot()
{
  char mac[20];
  sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
  server.send(200, "text/plain", mac);
}

void handleSwitch()
{
  static int i = 0;
  if (i++ % 2 == 0)
  {
    digitalWrite(LED, LOW);
    digitalWrite(SWITCH, LOW);
    server.send(200, "text/plain", "Close");
  }
  else
  {
    digitalWrite(LED, HIGH);
    digitalWrite(SWITCH, HIGH);
    server.send(200, "text/plain", "Open");
  }
}

void handleVersion()
{
  server.send(200, "text/plain", Version);
}

void handleNotFound()
{
  server.send(404, "text/plain", "File Not Found");
}

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(SWITCH, OUTPUT);
  Serial.begin(115200);
  WiFi.begin("bcs", "28911603");
  WiFi.macAddress(macAddr);
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.println("Wifi connecting...");
    notConnectedCounter++;
    if (notConnectedCounter > 150)
    { // Reset board if not connected after 5s
      Serial.println("Resetting due to Wifi not connecting...");
      ESP.restart();
    }
  }
  Serial.print("Wifi connected, IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.begin();

  // ping esp.local
  if (MDNS.begin("esp"))
  {
    Serial.println("MDNS responder started");
  }

  // curl http://esp.local/switch
  server.on("/", handleRoot);
  server.on("/switch", handleSwitch);
  server.on("/version", handleVersion);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop()
{
  ArduinoOTA.handle();
  server.handleClient();
}