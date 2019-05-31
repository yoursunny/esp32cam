#include <esp32cam.h>
#include <WebServer.h>
#include <WiFi.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";

WebServer server(80);

void printFrameInfo(const esp32cam::Frame& frame)
{
  Serial.print(frame.getWidth());
  Serial.print("x");
  Serial.print(frame.getHeight());
  Serial.print(" ");
  Serial.print(frame.size());
  Serial.println("b");
}

void handleBmp() {
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.print("CAPTURE OK ");
  printFrameInfo(*frame);

  if (!frame->toBmp()) {
    Serial.println("CONVERT FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.print("CONVERT OK ");
  printFrameInfo(*frame);

  server.setContentLength(frame->size());
  server.send(200, "image/bmp");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleJpg()
{
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.print("CAPTURE OK ");
  printFrameInfo(*frame);

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleMjpeg()
{
#define BOUNDARY "1306e191eccc45528d155f34bc1f6c84"
  WiFiClient client = server.client();
  client.print("HTTP/1.1 200 OK\r\nContent-Type: multipart/x-mixed-replace;boundary=" BOUNDARY "\r\n\r\n");
  while (true) {
    auto frame = esp32cam::capture();
    if (frame == nullptr) {
      Serial.println("CAPTURE FAIL");
      server.send(503, "", "");
      break;
    }
    Serial.print("CAPTURE OK ");
    printFrameInfo(*frame);

    client.print("Content-Type: image/jpeg\r\nContent-Length: ");
    client.print(frame->size());
    client.print("\r\n\r\n");
    if (!frame->writeTo(client)) {
      break;
    }
    client.print("\r\n--" BOUNDARY "\r\n");
  }
#undef BOUNDARY
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(Resolution::find(640, 480));
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.println("  /cam.bmp");
  Serial.println("  /cam.jpg");
  Serial.println("  /cam.mjpeg");

  server.on("/cam.bmp", handleBmp);
  server.on("/cam.jpg", handleJpg);
  server.on("/cam.mjpeg", handleMjpeg);

  server.begin();
}

void loop()
{
  server.handleClient();
}
