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

  struct Resolution
  {
    int width;
    int height;
  };

  explicit
  OV2640(const Pins& pins);

  ~OV2640();

  bool begin();

  bool capture();

  Resolution getResolution() const;

  const uint8_t* getFrameBuffer() const;

  size_t sizeofFrameBuffer() const;

  bool writeBmp(Print& os) const;

private:
  Pins* m_pins;
};
