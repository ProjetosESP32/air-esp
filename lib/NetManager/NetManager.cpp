#include "NetManager.hpp"

#include <string>

#include <Arduino.h>
#include <WiFi.h>

NetManager::NetManager(int port, const std::string _ssid, const std::string _password) : server(port)
{
  ssid = _ssid;
  password = _password;
}

void NetManager::setup()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), password.c_str());

#ifdef NET_DEBUG
  auto ip = WiFi.softAPIP();
  Serial.println(ip);
#endif

  server.begin();
}

void NetManager::serverTick(void (*onReceive)(int), int (*onSend)(void))
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
