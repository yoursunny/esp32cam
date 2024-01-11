#include "SpiRamOStream.hpp"

#include <HTTPClient.h>
#include <WiFi.h>
#include <odroid_go.h>

static const char* WIFI_SSID = "my-ssid";
static const char* WIFI_PASS = "my-pass";
static const char* CAM_SERVER = "192.0.2.1";
static const uint16_t CAM_PORT = 80;
static const char* CAM_URI = "/320x240.jpg";

void
setup() {
  GO.begin(115200);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi failure");
    delay(5000);
    ESP.restart();
  }
}

void
loop() {
  WiFiClient tcp;
  HTTPClient http;
  http.begin(tcp, CAM_SERVER, CAM_PORT, CAM_URI);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  if (httpCode != 200) {
    Serial.printf("HTTP error %d\n", httpCode);
    return;
  }

  int length = http.getSize();
  if (length <= 0) {
    Serial.printf("%lu JPEG length unknown\n", millis());
    return;
  }
  Serial.printf("%lu JPEG len=%d\n", millis(), length);

  SpiRamOStream os(length);
  http.writeToStream(&os);
  GO.lcd.drawJpg(os.data(), os.size(), 0, 0, 0, 0, 0, 0, JPEG_DIV_NONE);
}
