#ifndef ESP32CAM_MJPEG_HPP
#define ESP32CAM_MJPEG_HPP

#include "frame.hpp"

#include <memory>

namespace esp32cam {

/** MJPEG stream options. */
struct MjpegConfig {
  /**
   * @brief Minimum interval between frame captures in millis.
   *
   * Negative value causes @c asyncweb::MjpegResponse to enter continuous mode.
   */
  int minInterval = 0;

  /**
   * @brief Maximum number of frames before disconnecting.
   *
   * Negative value means unlimited.
   */
  int maxFrames = -1;

  /** @brief Time limit of writing one frame in millis. */
  int frameTimeout = 10000;
};

namespace detail {

/** @brief Control MJPEG stream timing. */
class MjpegController {
public:
  explicit MjpegController(MjpegConfig cfg);

  /** @brief Retrieve number of sent frames. */
  int countSentFrames() const {
    return m_count;
  }

  /**
   * @brief Decide what to do now.
   * @retval CAPTURE capture a frame.
   * @retval RETURN return a captured frame.
   * @retval SEND send current frame to the client.
   * @retval STOP disconnect the client.
   * @return if non-negative, how long to delay (millis) before the next action.
   */
  int decideAction();

  /**
   * @brief Notify that frame capture has started.
   * @post decideAction()==RETURN
   */
  void notifyCapture();

  /**
   * @brief Notify that frame capture has completed.
   * @param frame captured frame, possibly nullptr.
   * @post if frame==nullptr, decideAction()==STOP; otherwise, decideAction()==SEND
   */
  void notifyReturn(std::unique_ptr<Frame> frame);

  /** @brief Retrieve current frame. */
  Frame* getFrame() const {
    return m_frame.get();
  }

  /**
   * @brief Notify that a frame is sent to the client.
   * @param ok whether sent successfully.
   * @post getFrame()==nullptr
   *
   * The caller is expected to enforce @c MjpegConfig::frameTimeout and call
   * `notifySent(false)` in case of send timeout.
   */
  void notifySent(bool ok);

  /**
   * @brief Notify that an error has occurred.
   * @post getFrame()==nullptr
   * @post decideAction()==STOP
   */
  void notifyFail();

public:
  enum Action {
    CAPTURE = -1,
    RETURN = -2,
    SEND = -3,
    STOP = -4,
  };

  const MjpegConfig cfg;

private:
  std::unique_ptr<Frame> m_frame;
  unsigned long m_nextCaptureTime;
  int m_nextAction = CAPTURE;
  int m_count = 0;
};

/** @brief Prepare HTTP headers related to MJPEG streaming. */
class MjpegHeader {
public:
  void prepareResponseHeaders();

  void prepareResponseContentType();

  void preparePartHeader(size_t contentLength);

  void preparePartTrailer();

  size_t writeTo(Print& os);

public:
  size_t size = 0;
  char buf[120];
};

} // namespace detail
} // namespace esp32cam

#endif // ESP32CAM_MJPEG_HPP
