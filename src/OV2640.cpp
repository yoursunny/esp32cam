#include "OV2640.h"
extern "C" {
#include "internal/bitmap.h"
}
#include <esp_camera.h>

static framesize_t
getFrameSizeFromResolution(OV2640::Resolution resolution)
{
  switch (resolution) {
    case OV2640::Resolution::R_160x120:
      return FRAMESIZE_QQVGA;
    case OV2640::Resolution::R_320x240:
      return FRAMESIZE_QVGA;
    case OV2640::Resolution::R_640x480:
      return FRAMESIZE_VGA;
    case OV2640::Resolution::R_800x600:
      return FRAMESIZE_SVGA;
    case OV2640::Resolution::R_1280x1024:
      return FRAMESIZE_SXGA;
    case OV2640::Resolution::R_1600x1200:
      return FRAMESIZE_UXGA;
  }
  return FRAMESIZE_QVGA;
}

OV2640::OV2640(const Pins& pins)
  : m_pins(new Pins())
  , m_fb(nullptr)
{
  *m_pins = pins;
}

OV2640::~OV2640()
{
  if (m_pins != nullptr) {
    delete m_pins;
    return;
  }

  if (m_fb != nullptr) {
    esp_camera_fb_return(static_cast<camera_fb_t*>(m_fb));
  }
  esp_camera_deinit();
}

bool
OV2640::begin(Resolution resolution)
{
  if (m_pins == nullptr) {
    // already initialized
    return true;
  }

  camera_config_t cfg = { 0 };
  cfg.pin_pwdn = m_pins->RESET;
  cfg.pin_reset = -1;
  cfg.pin_xclk = m_pins->XCLK;
  cfg.pin_sscb_sda = m_pins->SDA;
  cfg.pin_sscb_scl = m_pins->SCL;
  cfg.pin_d7 = m_pins->D7;
  cfg.pin_d6 = m_pins->D6;
  cfg.pin_d5 = m_pins->D5;
  cfg.pin_d4 = m_pins->D4;
  cfg.pin_d3 = m_pins->D3;
  cfg.pin_d2 = m_pins->D2;
  cfg.pin_d1 = m_pins->D1;
  cfg.pin_d0 = m_pins->D0;
  cfg.pin_vsync = m_pins->VSYNC;
  cfg.pin_href = m_pins->HREF;
  cfg.pin_pclk = m_pins->PCLK;
  cfg.xclk_freq_hz = 7000000;
  cfg.ledc_timer = LEDC_TIMER_0;
  cfg.ledc_channel = LEDC_CHANNEL_0;
  cfg.pixel_format = PIXFORMAT_RGB888;
  cfg.frame_size = getFrameSizeFromResolution(resolution);
  cfg.jpeg_quality = 0;
  cfg.fb_count = 1;

  if (esp_camera_init(&cfg) != ESP_OK) {
    return false;
  }
  delete m_pins;
  return true;
}

bool
OV2640::capture()
{
  if (m_fb != nullptr) {
    esp_camera_fb_return(static_cast<camera_fb_t*>(m_fb));
    m_fb = nullptr;
  }
  m_fb = esp_camera_fb_get();
  return m_fb != nullptr;
}

int
OV2640::getWidth() const
{
  if (m_fb == nullptr) {
    return 0;
  }
  return static_cast<camera_fb_t*>(m_fb)->width;
}

int
OV2640::getHeight() const
{
  if (m_fb == nullptr) {
    return 0;
  }
  return static_cast<camera_fb_t*>(m_fb)->height;
}

const uint8_t*
OV2640::getFrameBuffer() const
{
  if (m_fb == nullptr) {
    return nullptr;
  }
  return static_cast<camera_fb_t*>(m_fb)->buf;
}

size_t
OV2640::sizeofFrameBuffer() const
{
  if (m_fb == nullptr) {
    return 0;
  }
  return static_cast<camera_fb_t*>(m_fb)->len;
}

bool
OV2640::writeBmp(Print& os, int timeout) const
{
  bitmap_header_t* header =
    bmp_create_header(this->getWidth(), this->getHeight());
  if (header == nullptr) {
    return false;
  }
  os.write(reinterpret_cast<const uint8_t*>(header), sizeof(*header));
  free(header);

  unsigned long startTime = millis();
  const uint8_t* fb = this->getFrameBuffer();
  for (size_t i = 0, sz = this->sizeofFrameBuffer(); i < sz;
       i += os.write(&fb[i], sz - i)) {
    if (millis() - startTime > static_cast<unsigned long>(timeout)) {
      return false;
    }
    yield();
  }
  return true;
}
