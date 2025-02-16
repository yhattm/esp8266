#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

const char *Version = "8.2.15";

const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "yhattm/iot";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

#define LED 2
#define SWITCH 0
ESP8266WebServer server(80);
uint8_t macAddr[6];

void openDoor()
{
  digitalWrite(LED, HIGH);
  digitalWrite(SWITCH, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  digitalWrite(SWITCH, LOW);
}

void handleRoot()
{
  char mac[20];
  sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
  server.send(200, "text/plain", mac);
}

void handleSwitch()
{
  openDoor();
  server.send(200, "text/plain", "Open");
}

void handleVersion()
{
  server.send(200, "text/plain", Version);
}

void handleNotFound()
{
  server.send(404, "text/plain", "File Not Found");
}

void reconnect()
{
  String client_id = "esp8266-client-";
  client_id += String(random(0xffff), HEX);
  if (client.connect(client_id.c_str()))
  {
    client.subscribe(topic);
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  openDoor();
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
  server.on("/open", handleSwitch);
  server.on("/version", handleVersion);
  server.onNotFound(handleNotFound);
  server.begin();

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
}

void loop()
{
  ArduinoOTA.handle();
  server.handleClient();
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}