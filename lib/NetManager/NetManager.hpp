#ifndef _NETMANAGER_HPP_
#define _NETMANAGER_HPP_

#include <Arduino.h>
#include <WiFi.h>

class NetManager
{
private:
  int port;
  String ssid;
  String password;
  WiFiServer server;
  void (*onReceive)(int);
  int (*onSend)(void);

public:
  NetManager(int port, const String ssid, const String password);
  void setup(void (*onReceive)(int), int (*onSend)(void));
  void loop();
};

#endif
