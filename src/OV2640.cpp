#include "OV2640.h"
#include "internal/camera.h"
extern "C" {
#include "internal/bitmap.h"
}

OV2640::OV2640(const Pins& pins)
  : m_pins(new Pins())
{
  *m_pins = pins;
}

OV2640::~OV2640()
{
  if (m_pins != nullptr) {
    delete m_pins;
  }
}

bool
OV2640::begin()
{
  if (m_pins == nullptr) {
    // already initialized
    return true;
  }

  camera_config_t cfg = {0};
  cfg.ledc_channel = LEDC_CHANNEL_0;
  cfg.ledc_timer = LEDC_TIMER_0;
  cfg.pin_d0 = m_pins->D0;
  cfg.pin_d1 = m_pins->D1;
  cfg.pin_d2 = m_pins->D2;
  cfg.pin_d3 = m_pins->D3;
  cfg.pin_d4 = m_pins->D4;
  cfg.pin_d5 = m_pins->D5;
  cfg.pin_d6 = m_pins->D6;
  cfg.pin_d7 = m_pins->D7;
  cfg.pin_xclk = m_pins->XCLK;
  cfg.pin_pclk = m_pins->PCLK;
  cfg.pin_vsync = m_pins->VSYNC;
  cfg.pin_href = m_pins->HREF;
  cfg.pin_sscb_sda = m_pins->SDA;
  cfg.pin_sscb_scl = m_pins->SCL;
  cfg.pin_reset = m_pins->RESET;
  cfg.xclk_freq_hz = 20000000;

  camera_model_t model;
  esp_err_t err = camera_probe(&cfg, &model);
  if (err != ESP_OK || model != CAMERA_OV2640) {
    return false;
  }

  cfg.frame_size = CAMERA_FS_QQVGA;
  cfg.pixel_format = CAMERA_PF_RGB565;
  err = camera_init(&cfg);
  if (err != ESP_OK) {
    return false;
  }

  delete m_pins;
  return true;
}

bool
OV2640::capture()
{
  esp_err_t err = camera_run();
  return err == ESP_OK;
}

OV2640::Resolution
OV2640::getResolution() const
{
  return Resolution{camera_get_fb_width(), camera_get_fb_height()};
}

const uint8_t*
OV2640::getFrameBuffer() const
{
  return camera_get_fb();
}

size_t
OV2640::sizeofFrameBuffer() const
{
  return camera_get_data_size();
}

bool
OV2640::writeBmp(Print& os) const
{
  auto res = this->getResolution();
  bitmap_header_t* header = bmp_create_header(res.width, res.height);
  if (header == nullptr) {
    return false;
  }
  os.write(reinterpret_cast<const uint8_t*>(header), sizeof(*header));
  free(header);
  os.write(this->getFrameBuffer(), this->sizeofFrameBuffer());
  return true;
}
