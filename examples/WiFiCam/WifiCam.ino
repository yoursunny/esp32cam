#include <OV2640.h>
#include <WebServer.h>
#include <WiFi.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";

WebServer server(80);

OV2640 camera(OV2640::Pins{D0: 5, D1: 18, D2: 19, D3: 21, D4: 36, D5: 39, D6: 34, D7: 35,
                           XCLK: 0, PCLK: 22, VSYNC: 25, HREF: 23, SDA: 26, SCL: 27, RESET: 32,});

void handleBmp() {
  bool ok = camera.capture();
  Serial.println(ok ? "CAPTURE OK" : "CAPTURE FAIL");
  if (!ok) {
    server.send(503, "", "");
    return;
  }

  server.sendHeader("Content-Type", "image/bmp");
  WiFiClient client = server.client();
  camera.writeBmp(client);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  bool ok = camera.begin(OV2640::R_240x160);
  Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/bmp");

  server.on("/bmp", handleBmp);
  server.begin();
}

void loop() {
  server.handleClient();
}
