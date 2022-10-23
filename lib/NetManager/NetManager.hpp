#ifndef _NETMANAGER_HPP_
#define _NETMANAGER_HPP_

#include <string>

#include <Arduino.h>
#include <WiFi.h>

class NetManager
{
private:
  int port;
  std::string ssid;
  std::string password;
  WiFiServer server;

public:
  NetManager(int port, const std::string ssid, const std::string password);
  void setup();
  void serverTick(void (*onReceive)(int), int (*onSend)(void));
};

#endif
