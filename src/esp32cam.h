#ifndef ESP32CAM_H
#define ESP32CAM_H

#include "internal/config.hpp"
#include "internal/frame.hpp"

#include <memory>

namespace esp32cam {

class CameraClass
{
public:
  /** \brief Enable camera.
   */
  bool
  begin(const Config& config);

  /** \brief Disable camera.
   */
  bool
  end();

  /** \brief Change camera resolution.
   *  \pre Initial resolution is higher than specified resolution.
   *  \param resolution new resolution
   *  \param sleepFor how long to wait for stabilization (millis)
   */
  bool
  changeResolution(const Resolution& resolution, int sleepFor = 500);

  /** \brief Capture a frame of picture.
   */
  std::unique_ptr<Frame>
  capture();

  struct StreamMjpegConfig
  {
    /// minimum interval between frame captures.
    int minInterval = 0;
    /// maximum number of frames before disconnecting.
    int maxFrames = -1;
    /// time limit of writing one frame in millis.
    int frameTimeout = 10000;
  };

  /** \brief Stream Motion JPEG.
   *  \pre The camera has been initialized to JPEG mode.
   *  \return number of frames streamed.
   */
  int
  streamMjpeg(Client& client, const StreamMjpegConfig& cfg);

  int
  streamMjpeg(Client& client)
  {
    return streamMjpeg(client, StreamMjpegConfig());
  }
};

extern CameraClass Camera;

inline std::unique_ptr<Frame>
capture()
{
  return Camera.capture();
}

} // namespace esp32cam

#endif // ESP32CAM_H
