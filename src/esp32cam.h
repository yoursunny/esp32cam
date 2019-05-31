#ifndef ESP32CAM_H
#define ESP32CAM_H

#include "internal/config.hpp"
#include "internal/frame.hpp"

#include <memory>

namespace esp32cam {

class CameraClass
{
public:
  bool
  begin(const Config& config);

  bool
  end();

  std::unique_ptr<Frame>
  capture();
};

extern CameraClass Camera;

inline std::unique_ptr<Frame>
capture()
{
  return Camera.capture();
}

} // namespace esp32cam

#endif // ESP32CAM_H
