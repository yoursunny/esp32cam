#include "esp32cam.h"

#include <esp_camera.h>

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

std::unique_ptr<Frame>
CameraClass::capture()
{
  camera_fb_t* fb = esp_camera_fb_get();
  if (fb == nullptr) {
    return nullptr;
  }
  return std::unique_ptr<Frame>(new Frame(fb));
}

} // namespace esp32cam
