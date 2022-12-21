#include "NetManager.hpp"

#include <Arduino.h>
#include <WiFi.h>

NetManager::NetManager(int port, const String _ssid, const String _password) : server(port)
{
  ssid = _ssid;
  password = _password;
}

void NetManager::setup(void (*_onReceive)(int), int (*_onSend)(void))
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), password.c_str());
  onReceive = _onReceive;
  onSend = _onSend;

  auto ip = WiFi.softAPIP();
  Serial.println(ip);

  server.begin();
}

void NetManager::loop()
{
  auto client = server.available();
  int dataReceived = 0;

  if (client)
  {
    if (client.available()) {
      dataReceived = client.parseInt();
      onReceive(dataReceived);
    }

    client.printf("%d\n", onSend());
    client.stop();
  }
}
