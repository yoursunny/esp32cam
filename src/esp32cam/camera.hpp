#ifndef ESP32CAM_CAMERA_HPP
#define ESP32CAM_CAMERA_HPP

#include "config.hpp"
#include "mjpeg.hpp"
#include "setters.hpp"

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
   */
  bool changeResolution(const Resolution& resolution, int sleepFor = 500);

  /**
   * @brief Update camera configuration.
   * @param setter See `setters.hpp`.
   * @return whether success.
   *
   * Example:
   * @code
   * bool ok = Camera.update(SetVflip(true));
   * @endcode
   */
  bool update(const SensorSetter& setter);

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
