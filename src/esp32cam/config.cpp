#include "config.hpp"

#include <algorithm>
#include <cstring>
#include <esp_camera.h>

namespace esp32cam {
namespace detail {

int
convertJpegQuality(int quality) {
  return std::min(63, std::max(0, 100 - quality));
}

} // namespace detail

class Config::CameraConfigT : public camera_config_t {};

Config::Config()
  : m_cfg(new CameraConfigT) {
  std::memset(m_cfg, 0, sizeof(*m_cfg));
  m_cfg->xclk_freq_hz = 20000000;
  m_cfg->ledc_timer = LEDC_TIMER_0;
  m_cfg->ledc_channel = LEDC_CHANNEL_0;
  m_cfg->pixel_format = PIXFORMAT_RGB565;
  m_cfg->frame_size = FRAMESIZE_QQVGA;
  m_cfg->fb_count = 2;
  m_cfg->grab_mode = CAMERA_GRAB_LATEST;
}

Config::~Config() {
  delete m_cfg;
}

Config&
Config::setPins(const Pins& pins) {
  m_cfg->pin_pwdn = pins.PWDN;
  m_cfg->pin_reset = pins.RESET;
  m_cfg->pin_xclk = pins.XCLK;
  m_cfg->pin_sccb_sda = pins.SDA;
  m_cfg->pin_sccb_scl = pins.SCL;
  m_cfg->pin_d7 = pins.D7;
  m_cfg->pin_d6 = pins.D6;
  m_cfg->pin_d5 = pins.D5;
  m_cfg->pin_d4 = pins.D4;
  m_cfg->pin_d3 = pins.D3;
  m_cfg->pin_d2 = pins.D2;
  m_cfg->pin_d1 = pins.D1;
  m_cfg->pin_d0 = pins.D0;
  m_cfg->pin_vsync = pins.VSYNC;
  m_cfg->pin_href = pins.HREF;
  m_cfg->pin_pclk = pins.PCLK;
  return *this;
}

Config&
Config::setResolution(const Resolution& resolution) {
  m_cfg->frame_size = resolution.as<framesize_t>();
  return *this;
}

Config&
Config::setBufferCount(int n) {
  m_cfg->fb_count = std::max(1, n);
  return *this;
}

Config&
Config::setRgb() {
  m_cfg->pixel_format = PIXFORMAT_RGB565;
  return *this;
}

Config&
Config::setYuv() {
  m_cfg->pixel_format = PIXFORMAT_YUV422;
  return *this;
}

Config&
Config::setGrayscale() {
  m_cfg->pixel_format = PIXFORMAT_GRAYSCALE;
  return *this;
}

Config&
Config::setJpeg(int quality) {
  m_cfg->pixel_format = PIXFORMAT_JPEG;
  m_cfg->jpeg_quality = detail::convertJpegQuality(quality);
  return *this;
}

} // namespace esp32cam
