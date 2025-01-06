/**
 * @mainpage esp32cam
 *
 * https://github.com/yoursunny/esp32cam
 */

#ifndef ESP32CAM_H
#define ESP32CAM_H

#include "esp32cam/asyncweb.hpp"
#include "esp32cam/camera.hpp"
#include "esp32cam/logger.hpp"

namespace esp32cam {

/** @brief Capture a frame with the camera. */
inline std::unique_ptr<Frame>
capture() {
  return Camera.capture();
}

} // namespace esp32cam

#endif // ESP32CAM_H
