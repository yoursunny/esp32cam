#include <Arduino.h>
#include "internal/camera.h"

class OV2640
{
public:
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
  };

  enum Resolution {
    R_40x30,
    R_64x32,
    R_64x64,
    R_88x72,
    R_160x120,
    R_128x160,
    R_176x144,
    R_240x160,
    R_320x240,
    R_352x288,
    R_640x480,
    R_800x600,
    R_1280x1024,
    R_1600x1200,
  };

  explicit
  OV2640(const Pins& pins);

  ~OV2640();

  bool begin(Resolution resolution = R_160x120);

  bool capture();

  int getWidth() const;
  int getHeight() const;

  const uint8_t* getFrameBuffer() const;
  size_t sizeofFrameBuffer() const;

  bool writeBmp(Print& os, int timeout = 4000) const;

private:
  Pins* m_pins;
};
