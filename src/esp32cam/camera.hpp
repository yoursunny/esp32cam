#ifndef ESP32CAM_CAMERA_HPP
#define ESP32CAM_CAMERA_HPP

#include "config.hpp"
#include "mjpeg.hpp"
#include <functional>

namespace esp32cam {

class CameraClass {
public:
  /**
   * @brief Enable camera.
   * @return whether success.
   */
  bool begin(const Config& config);

  /**
   * @brief Disable camera.
   * @return whether success.
   */
  bool end();

  /**
   * @brief Retrieve list of resolutions (likely) supported by hardware.
   * @pre Camera is enabled.
   */
  ResolutionList listResolutions() const;

  /**
   * @brief Change camera resolution.
   * @pre Camera is enabled.
   * @param resolution new resolution; must be no higher than initial resolution.
   * @param sleepFor how long to wait for stabilization (millis).
   * @deprecated Use @c update instead.
   */
  bool changeResolution(const Resolution& resolution, int sleepFor = 500);

  /**
   * @brief Retrieve runtime settings.
   * @pre Camera is enabled.
   */
  Settings status() const;

  /**
   * @brief Update runtime settings.
   * @param settings updated settings.
   * @param sleepFor how long to wait for stabilization (millis).
   * @pre Camera is enabled.
   * @post Camera may be left in inconsistent state in case of failure.
   * @return whether success.
   */
  bool update(const Settings& settings, int sleepFor = 0);

  /**
   * @brief Update runtime settings using modifier function.
   * @param modifier function to modify settings.
   * @param sleepFor how long to wait for stabilization (millis).
   * @pre Camera is enabled.
   * @post Camera may be left in inconsistent state in case of failure.
   * @return whether success.
   */
  template<typename Fn>
  std::enable_if_t<std::is_assignable_v<std::function<void(Settings&)>, Fn>, bool> //
  update(const Fn& modifier, int sleepFor = 0) {
    Settings settings = status();
    modifier(settings);
    return update(settings, sleepFor);
  }

  /**
   * @brief Capture a frame of picture.
   * @pre Camera is enabled.
   * @return the picture frame, or nullptr on error.
   */
  std::unique_ptr<Frame> capture();

  /**
   * @brief Stream Motion JPEG.
   * @pre The camera has been initialized to JPEG mode.
   * @return number of frames streamed.
   */
  int streamMjpeg(Client& client, const MjpegConfig& cfg = MjpegConfig());
};

/** @brief ESP32 camera API. */
extern CameraClass Camera;

} // namespace esp32cam

#endif // ESP32CAM_CAMERA_HPP
