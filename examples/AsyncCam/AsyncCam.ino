#include <esp32cam-asyncweb.h>

#include <ESPAsyncWebServer.h>
#include <WiFi.h>

static const char* WIFI_SSID = "my-ssid";
static const char* WIFI_PASS = "my-pass";

AsyncWebServer server(80);

void
setup()
{
  Serial.begin(115200);
  Serial.println();

  {
    using namespace esp32cam;

    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(Resolution::find(1024, 768));
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    if (!ok) {
      Serial.println("camera initialize failure");
      delay(5000);
      ESP.restart();
    }
    Serial.println("camera initialize success");
  }

  // stabilize camera before starting WiFi to reduce "Brownout detector was triggered"
  delay(2000);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi failure");
    delay(5000);
    ESP.restart();
  }

  Serial.println("WiFi connected");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/still.jpg");

  server.on("/still.jpg", esp32cam::asyncweb::handleStill);

  server.begin();
}

void
loop()
{
  // esp32cam-asyncweb.h depends on FreeRTOS task API including vTaskDelete, so you must have a
  // non-zero delay in the loop() function; otherwise, FreeRTOS kernel memory cannot be freed
  // properly and the system would run out of memory.
  delay(1);
}
