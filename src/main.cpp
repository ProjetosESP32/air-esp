#include <Arduino.h>
#include <WiFi.h>
#include "NetManager.hpp"

constexpr char SSID[] = "ESP32Server";
constexpr char PASSWORD[] = "12345678";
constexpr int SERVER_PORT = 5000;

NetManager netManager(SERVER_PORT, SSID, PASSWORD);

int temperature = 24;

void handleCommandReceive(int command)
{
  temperature = command;
}

int handleCommandSend()
{
  return temperature;
}

void setup()
{
  Serial.begin(115200);

  netManager.setup(handleCommandReceive, handleCommandSend);
}

void loop()
{
  netManager.loop();

  delay(10);
}
