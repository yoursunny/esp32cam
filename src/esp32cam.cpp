#include "esp32cam.h"

#include <Arduino.h>
#include <esp_camera.h>

#define BOUNDARY "e8b8c539-047d-4777-a985-fbba6edff11e"
// #undef BOUNDARY

namespace esp32cam {

CameraClass Camera;

bool
CameraClass::begin(const Config& config)
{
  return esp_camera_init(reinterpret_cast<const camera_config_t*>(config.m_cfg)) == ESP_OK;
}

bool
CameraClass::end()
{
  return esp_camera_deinit() == ESP_OK;
}

bool
CameraClass::changeResolution(const Resolution& resolution, int sleepFor)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  framesize_t frameSize = resolution.as<framesize_t>();
  if (sensor->status.framesize == frameSize) {
    return true;
  }

  if (sensor->set_framesize(sensor, frameSize) != 0) {
    return false;
  }
  delay(sleepFor);
  return true;
}

std::unique_ptr<Frame>
CameraClass::capture()
{
  camera_fb_t* fb = esp_camera_fb_get();
  if (fb == nullptr) {
    return nullptr;
  }
  return std::unique_ptr<Frame>(new Frame(fb));
}

int CameraClass::streamMjpeg(Client& client, const StreamMjpegConfig& cfg) {
  client.print("HTTP/1.1 200 OK\r\n"
               "Content-Type: multipart/x-mixed-replace;boundary=" BOUNDARY "\r\n"
               "\r\n");
  auto lastCapture = millis();
  int nFrames;
  for (nFrames = 0; cfg.maxFrames < 0 || nFrames < cfg.maxFrames; ++nFrames) {
    auto now = millis();
    auto sinceLastCapture = now - lastCapture;
    if (static_cast<int>(sinceLastCapture) < cfg.minInterval) {
      delay(cfg.minInterval - sinceLastCapture);
    }
    lastCapture = millis();

    auto frame = capture();
    if (frame == nullptr) {
      break;
    }

    client.printf("Content-Type: image/jpeg\r\n"
                  "Content-Length: %d\r\n"
                  "\r\n",
                  static_cast<int>(frame->size()));
    if (!frame->writeTo(client, cfg.frameTimeout)) {
      break;
    }
    client.print("\r\n--" BOUNDARY "\r\n");
    yield();
  }
  return nFrames;
}

/*
Cloned function for ASYNCCLIENT
*/
int CameraClass::streamMjpeg(AsyncClient& client, const StreamMjpegConfig& cfg) {
  client.write("HTTP/1.1 200 OK\r\n"
               "Content-Type: multipart/x-mixed-replace;boundary=" BOUNDARY "\r\n"
               "\r\n");
  auto lastCapture = millis();
  int nFrames;
  for (nFrames = 0; cfg.maxFrames < 0 || nFrames < cfg.maxFrames; ++nFrames) {
    auto now = millis();
    auto sinceLastCapture = now - lastCapture;
    if (static_cast<int>(sinceLastCapture) < cfg.minInterval) {
      delay(cfg.minInterval - sinceLastCapture);
    }
    lastCapture = millis();

    auto frame = capture();
    if (frame == nullptr) {
      break;
    }

    char szTmp[256];
    size_t stLen = snprintf_P(szTmp, 256, PSTR(
	"Content-Type: image/jpeg\r\n"
	"Content-Length: %d\r\n"
	"\r\n"), static_cast<int>(frame->size())
	);
    client.write((const char*) szTmp, stLen);
    if (!frame->writeTo(client, cfg.frameTimeout)) {
      break;
    }
    client.write("\r\n--" BOUNDARY "\r\n");
    yield();
  }
  return nFrames;
}



} // namespace esp32cam
