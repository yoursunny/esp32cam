#ifndef ESP32CAM_LOGGER_HPP
#define ESP32CAM_LOGGER_HPP

#include <Arduino.h>

namespace esp32cam {

extern Print* LogOutput;

/**
 * @brief Set logger output.
 * @param output Output printer, such as @c Serial .
 */
inline void
setLogger(Print& output) {
  LogOutput = &output;
}

#define ESP32CAM_LOG(fmt, ...)                                                                     \
  do {                                                                                             \
    if (LogOutput != nullptr) {                                                                    \
      LogOutput->printf("[%8lu] " fmt "\n", millis(), ##__VA_ARGS__);                              \
    }                                                                                              \
  } while (false)

} // namespace esp32cam

#endif // ESP32CAM_LOGGER_HPP
