#include <Arduino.h>
#include <WiFi.h>
#include "Control_IR.hpp"
#define SSID "ESP32Server"
#define PASSWORD "12345678"
#define SERVER_PORT 80

WiFiServer server(SERVER_PORT);

int temperature = 24;

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASSWORD);

  IPAddress ip = WiFi.softAPIP();
  Serial.println(ip);

  server.begin();
}

void loop()
{
  WiFiClient client = server.available();

  if (client)
  {
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
      }
    }

    client.stop();
  }
}
