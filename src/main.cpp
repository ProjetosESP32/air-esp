#include <Arduino.h>
#include <WiFi.h>
#include <InfraRed.hpp>
#define SSID "ESP32Server"
#define PASSWORD "12345678"
#define SERVER_PORT 80

InfraRed Infra;

WiFiServer server(SERVER_PORT);

void setup()
{
  Serial.begin(115200);
  Serial.println("Iniciando");
  Serial.setTimeout(10);

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
      Infra.recRaws();
      Serial.print("Gravou");
      return;
    }

    if (str == "liga")
    {
      Infra.sendPower(true);
      return;
    }

    if (str == "desliga")
    {
      Infra.sendPower(false);
      return;
    }

    int temperature = str.toInt();
    Infra.sendTemperature(temperature);
  }

  delay(1);
}
