#include "SpiRamOStream.hpp"

#include <HTTPClient.h>
#include <WiFi.h>
#include <odroid_go.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";
const char* CAM_SERVER = "http://192.0.2.1";

void
setup()
{
  GO.begin(115200);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void
loop()
{
  WiFiClient tcp;
  HTTPClient http;
  http.begin(tcp, CAM_SERVER + String("/cam-lo.jpg"));
  int httpCode = http.GET();
  if (httpCode != 200) {
    Serial.printf("HTTP error %d\n", httpCode);
    return;
  }

  int length = http.getSize();
  if (length <= 0) {
    Serial.println("JPEG length unknown");
    return;
  }
  Serial.printf("JPEG len=%d\n", length);

  SpiRamOStream os(length);
  http.writeToStream(&os);
  GO.lcd.drawJpg(os.data(), os.size(), 0, 0, 0, 0, 0, 0, JPEG_DIV_NONE);
}