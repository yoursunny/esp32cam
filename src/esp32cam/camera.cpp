#include "camera.hpp"

#include <Arduino.h>
#include <esp_camera.h>

namespace esp32cam {

Print* LogOutput = nullptr;
CameraClass Camera;

bool
CameraClass::begin(const Config& config) {
  return esp_camera_init(reinterpret_cast<const camera_config_t*>(config.m_cfg)) == ESP_OK;
}

bool
CameraClass::end() {
  return esp_camera_deinit() == ESP_OK;
}

ResolutionList
CameraClass::listResolutions() const {
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return ResolutionList();
  }

  camera_sensor_info_t* info = esp_camera_sensor_get_info(&sensor->id);
  if (info == nullptr) {
    return ResolutionList();
  }

  return ResolutionList(info->max_size + 1);
}

bool
CameraClass::changeResolution(const Resolution& resolution, int sleepFor) {
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
  if (sleepFor > 0) {
    delay(sleepFor);
  }
  return true;
}

std::unique_ptr<Frame>
CameraClass::capture() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (fb == nullptr) {
    return nullptr;
  }
  return std::unique_ptr<Frame>(new Frame(fb));
}

int
CameraClass::streamMjpeg(Client& client, const MjpegConfig& cfg) {
  detail::MjpegHeader hdr;
  hdr.prepareResponseHeaders();
  hdr.writeTo(client);

  using Ctrl = detail::MjpegController;
  Ctrl ctrl(cfg);
  while (true) {
    auto act = ctrl.decideAction();
    switch (act) {
      case Ctrl::CAPTURE: {
        ctrl.notifyCapture();
        break;
      }
      case Ctrl::RETURN: {
        ctrl.notifyReturn(capture());
        break;
      }
      case Ctrl::SEND: {
        hdr.preparePartHeader(ctrl.getFrame()->size());
        hdr.writeTo(client);
        ctrl.notifySent(ctrl.getFrame()->writeTo(client, cfg.frameTimeout));
        hdr.preparePartTrailer();
        hdr.writeTo(client);
        break;
      }
      case Ctrl::STOP: {
        client.stop();
        return ctrl.countSentFrames();
      }
      default: {
        delay(act);
        break;
      }
    }
  }
}

} // namespace esp32cam
