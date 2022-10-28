#include <Arduino.h>
#include <WiFi.h>
#include "NetManager.hpp"

constexpr char SSID[] = "ESP32Server";
constexpr char PASSWORD[] = "12345678";
constexpr int SERVER_PORT = 80;

NetManager netManager(SERVER_PORT, SSID, PASSWORD);

int temperature = 24;

void setup()
{
  Serial.begin(115200);

  netManager.setup();
}

void loop()
{
  netManager.loop([](int newTemp)
                  { temperature = newTemp; },
                  []()
                  { return temperature; });

  delay(10);
}
