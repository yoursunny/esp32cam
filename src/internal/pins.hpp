#ifndef ESP32CAM_PINS_HPP
#define ESP32CAM_PINS_HPP

namespace esp32cam {

/** \brief Camera pins definition.
 */
struct Pins
{
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

constexpr Pins AiThinker{
  D0: 5, D1: 18, D2: 19, D3: 21, D4: 36, D5: 39, D6: 34, D7: 35,
  XCLK: 0, PCLK: 22, VSYNC: 25, HREF: 23, SDA: 26, SCL: 27, RESET: -1, PWDN: 32,
};

constexpr Pins M5Camera{
  D0: 32, D1: 35, D2: 34, D3: 5, D4: 39, D5: 18, D6: 36, D7: 19,
  XCLK: 27, PCLK: 21, VSYNC: 25, HREF: 26, SDA: 22, SCL: 23, RESET: 15, PWDN: -1
};

constexpr Pins M5CameraLED{
  // Red LED on GPIO 14, tally light when tied to PWDN
  D0: 32, D1: 35, D2: 34, D3: 5, D4: 39, D5: 18, D6: 36, D7: 19,
  XCLK: 27, PCLK: 21, VSYNC: 25, HREF: 26, SDA: 22, SCL: 23, RESET: 15, PWDN: 14
};


} // namespace pins
} // namespace esp32cam

#endif // ESP32CAM_PINS_HPP
