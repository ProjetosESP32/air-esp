#include <Arduino.h>
#include <WiFi.h>

constexpr char SSID[] = "ESP32Server";
constexpr char PASSWORD[] = "12345678";
constexpr int SERVER_PORT = 80;

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
    if (client.available())
    {
      temperature = client.parseInt();
    }

    client.printf("%d\n", temperature);

    client.stop();
  }
}
