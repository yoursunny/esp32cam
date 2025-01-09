#include "AsyncCam.hpp"
#include <WiFi.h>

static const char* WIFI_SSID = "my-ssid";
static const char* WIFI_PASS = "my-pass";

esp32cam::Resolution initialResolution;

AsyncWebServer server(80);

void
setup() {
  Serial.begin(115200);
  Serial.println();
  esp32cam::setLogger(Serial);
  delay(1000);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi failure %d\n", WiFi.status());
    delay(5000);
    ESP.restart();
  }
  Serial.println("WiFi connected");
  delay(1000);

  {
    using namespace esp32cam;

    initialResolution = Resolution::find(1024, 768);

    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(initialResolution);
    cfg.setBufferCount(3);
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    if (!ok) {
      Serial.println("camera initialize failure");
      delay(5000);
      ESP.restart();
    }
    Serial.println("camera initialize success");
  }

  Serial.println("camera starting");
  Serial.print("http://");
  Serial.println(WiFi.localIP());

  addRequestHandlers();
  server.begin();
}

void
loop() {
  // esp32cam/asyncweb.hpp depends on FreeRTOS task API including vTaskDelete, so you must have a
  // non-zero delay in the loop() function; otherwise, FreeRTOS kernel memory cannot be freed
  // properly and the system would run out of memory.
  delay(1);
}
