#ifndef ESP32CAM_PINS_HPP
#define ESP32CAM_PINS_HPP

namespace esp32cam {

/** @brief Camera pins definition. */
struct Pins {
  int D0;
  int D1;
  int D2;
  int D3;
  int D4;
  int D5;
  int D6;
  int D7;
  int XCLK;
  int PCLK;
  int VSYNC;
  int HREF;
  int SDA;
  int SCL;
  int RESET;
  int PWDN;
};

namespace pins {

/** @brief Pin definition for AI Thinker ESP32-CAM. */
constexpr Pins AiThinker{
  D0: 5,
  D1: 18,
  D2: 19,
  D3: 21,
  D4: 36,
  D5: 39,
  D6: 34,
  D7: 35,
  XCLK: 0,
  PCLK: 22,
  VSYNC: 25,
  HREF: 23,
  SDA: 26,
  SCL: 27,
  RESET: -1,
  PWDN: 32,
};

/** @brief Pin definition for Seeed Studio XIAO ESP32S3 Sense. */
constexpr Pins XiaoSense{
  D0: 15,
  D1: 17,
  D2: 18,
  D3: 16,
  D4: 14,
  D5: 12,
  D6: 11,
  D7: 48,
  XCLK: 10,
  PCLK: 13,
  VSYNC: 38,
  HREF: 47,
  SDA: 40,
  SCL: 39,
  RESET: -1,
  PWDN: -1,
};

/** @brief Pin definition for FREENOVE WROVER ESP32-CAM. */
constexpr Pins FreeNove{
  D0: 4,
  D1: 5,
  D2: 18,
  D3: 19,
  D4: 36,
  D5: 39,
  D6: 34,
  D7: 35,
  XCLK: 21,
  PCLK: 22,
  VSYNC: 25,
  HREF: 23,
  SDA: 26,
  SCL: 27,
  RESET: -1,
  PWDN: -1,
};

/** @brief Pin definition for M5Stack M5Camera. */
constexpr Pins M5Camera{
  D0: 32,
  D1: 35,
  D2: 34,
  D3: 5,
  D4: 39,
  D5: 18,
  D6: 36,
  D7: 19,
  XCLK: 27,
  PCLK: 21,
  VSYNC: 25,
  HREF: 26,
  SDA: 22,
  SCL: 23,
  RESET: 15,
  PWDN: -1,
};

/**
 * @brief Pin definition for M5Stack M5Camera with LED.
 *
 * Red LED on GPIO 14, tally light when tied to PWDN
 */
constexpr Pins M5CameraLED{
  D0: 32,
  D1: 35,
  D2: 34,
  D3: 5,
  D4: 39,
  D5: 18,
  D6: 36,
  D7: 19,
  XCLK: 27,
  PCLK: 21,
  VSYNC: 25,
  HREF: 26,
  SDA: 22,
  SCL: 23,
  RESET: 15,
  PWDN: 14,
};

/** @brief Pin definition for TTGO ESP32-CAM. */
constexpr Pins TTGO{
  D0: 5,
  D1: 14,
  D2: 4,
  D3: 15,
  D4: 37,
  D5: 38,
  D6: 36,
  D7: 39,
  XCLK: 32,
  PCLK: 19,
  VSYNC: 27,
  HREF: 25,
  SDA: 13,
  SCL: 12,
  RESET: -1,
  PWDN: -1,
};

} // namespace pins
} // namespace esp32cam

#endif // ESP32CAM_PINS_HPP
