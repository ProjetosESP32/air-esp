#include <Arduino.h>
#include <WiFi.h>
#include "Control_IR.hpp"
#define SSID "ESP32Server"
#define PASSWORD "12345678"
#define SERVER_PORT 80

InfraRed Infra;

WiFiServer server(SERVER_PORT);

// int temperature = 24;

void setup()
{
  Serial.begin(115200);
  Serial.println("Iniciando");
  Serial.setTimeout(10);
  /*WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASSWORD);

  IPAddress ip = WiFi.softAPIP();
  Serial.println(ip);

  server.begin();*/
  Infra.begin();
}

void loop()
{
  if (Serial.available())
  {
    String str = Serial.readString();

    if (str == "gravar")
    {
      Serial.println("Gravando");
      Infra.RecRaw();
      Serial.print("Gravou");
      return;
    }

    if (str == "liga")
    {
      Infra.power(true);
      return;
    }

    if (str == "desliga")
    {
      Infra.power(false);
      return;
    }

    if (str == "list")
    {
      Infra.listdir();
      return;
    }

    int temperature = str.toInt();
    temperature = max(temperature, 18);
    temperature = min(temperature, 25);
    Infra.setTemperature(temperature);
  }
  delay(1);

  /*WiFiClient client = server.available();

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
  }*/
}
