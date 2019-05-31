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

} // namespace pins
} // namespace esp32cam

#endif // ESP32CAM_PINS_HPP
