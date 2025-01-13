#include "config.hpp"
#include "camera.hpp"
#include "logger.hpp"

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

Settings
CameraClass::status() const {
  Settings result;
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return result;
  }

  const auto& ss = sensor->status;
  result.resolution = Resolution(ss.framesize);
  result.brightness = ss.brightness;
  result.contrast = ss.contrast;
  result.saturation = ss.saturation;
  result.lightMode = ss.awb_gain ? static_cast<LightMode>(ss.wb_mode) : LightMode::NONE;
  result.specialEffect = static_cast<SpecialEffect>(ss.special_effect);
  result.gain = ss.agc ? ((-2) << ss.gainceiling) : (1 + static_cast<int8_t>(ss.agc_gain));
  result.hmirror = ss.hmirror != 0;
  result.vflip = ss.vflip != 0;
  result.rawGma = ss.raw_gma != 0;
  result.lensCorrection = ss.lenc != 0;
  return result;
}

bool
CameraClass::update(const Settings& settings, int sleepFor) {
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

#define CHECK_RANGE(MEM, MIN, MAX)                                                                 \
  do {                                                                                             \
    int next = static_cast<int>(settings.MEM);                                                     \
    if (!(next >= MIN && next <= MAX)) {                                                           \
      ESP32CAM_LOG("update " #MEM " %d out of range [%d,%d]", next, MIN, MAX);                     \
      return false;                                                                                \
    }                                                                                              \
  } while (false)

#define UPDATE4(STATUS_MEM, value, SETTER_MEM, SETTER_TYP)                                         \
  do {                                                                                             \
    int prev = static_cast<int>(sensor->status.STATUS_MEM);                                        \
    int desired = static_cast<int>(value);                                                         \
    if (prev != desired) {                                                                         \
      int res = sensor->SETTER_MEM(sensor, static_cast<SETTER_TYP>(desired));                      \
      ESP32CAM_LOG("update " #STATUS_MEM " %d => %d %s", prev, desired,                            \
                   res == 0 ? "success" : "failure");                                              \
      if (res != 0) {                                                                              \
        return false;                                                                              \
      }                                                                                            \
    }                                                                                              \
  } while (false)
#define UPDATE3(STATUS_MEM, value, SETTER_TYP)                                                     \
  UPDATE4(STATUS_MEM, (value), set_##STATUS_MEM, SETTER_TYP)
#define UPDATE2(STATUS_MEM, value) UPDATE3(STATUS_MEM, (value), int)
#define UPDATE1(MEM) UPDATE2(MEM, settings.MEM)

  CHECK_RANGE(brightness, -2, 2);
  CHECK_RANGE(contrast, -2, 2);
  CHECK_RANGE(saturation, -2, 2);
  CHECK_RANGE(lightMode, -1, 4);
  CHECK_RANGE(specialEffect, 0, 6);
  CHECK_RANGE(gain, -128, 31);

  UPDATE3(framesize, settings.resolution.as<framesize_t>(), framesize_t);
  UPDATE1(brightness);
  UPDATE1(contrast);
  UPDATE1(saturation);
  UPDATE2(special_effect, settings.specialEffect);
  UPDATE1(hmirror);
  UPDATE1(vflip);
  UPDATE2(raw_gma, settings.rawGma);
  UPDATE2(lenc, settings.lensCorrection);

  if (settings.gain > 0) {
    UPDATE4(agc, 0, set_gain_ctrl, int);
    UPDATE2(agc_gain, settings.gain - 1);
    UPDATE3(gainceiling, 0, gainceiling_t);
  } else {
    UPDATE4(agc, 1, set_gain_ctrl, int);
    UPDATE2(agc_gain, 0);
    UPDATE3(gainceiling, __builtin_ctz(static_cast<uint32_t>(-settings.gain)) - 1, gainceiling_t);
  }

  if (settings.lightMode == LightMode::NONE) {
    UPDATE2(awb_gain, 0);
    UPDATE2(wb_mode, 0);
  } else {
    UPDATE2(awb_gain, 1);
    UPDATE2(wb_mode, settings.lightMode);
  }

#undef CHECK_RANGE
#undef UPDATE4
#undef UPDATE3
#undef UPDATE2
#undef UPDATE1

  if (sleepFor > 0) {
    delay(sleepFor);
  }
  return true;
}

} // namespace esp32cam
