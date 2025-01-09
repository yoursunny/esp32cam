#ifndef ESP32CAM_ASYNCWEB_HPP
#define ESP32CAM_ASYNCWEB_HPP

#if __has_include(<ESPAsyncWebServer.h>)
#include "camera.hpp"
#include <ESPAsyncWebServer.h>

namespace esp32cam {
namespace detail {

class CaptureTask {
public:
  explicit CaptureTask(uint32_t queueLength, uint32_t priority = 1);

  ~CaptureTask();

  explicit operator bool() const {
    return m_queue != nullptr && m_task != nullptr;
  }

  void request(bool continuous = false);

  std::unique_ptr<Frame> retrieve();

private:
  static void run(void* ctx);

private:
  void* m_queue = nullptr;
  void* m_task = nullptr;
  bool m_continuous = false;
};

} // namespace detail

/**
 * @brief esp32cam integration with ESPAsyncWebServer library.
 * @sa https://github.com/mathieucarbou/ESPAsyncWebServer
 */
namespace asyncweb {

/**
 * @brief HTTP response of one still image.
 *
 * Start a FreeRTOS task to capture an image frame under the current camera settings,
 * and respond to the HTTP request as a still JPEG image.
 * If multiple StillResponse instances are active concurrently, each will get a different image.
 * If task creation fails, respond with HTTP 500 error.
 * If image capture fails, the response would be empty, but still has HTTP 200 due to
 * ESPAsyncWebServer library limitations.
 */
class StillResponse : public AsyncAbstractResponse {
public:
  explicit StillResponse();

  ~StillResponse() override;

  bool _sourceValid() const override {
    return true;
  }

  size_t _fillBuffer(uint8_t* buf, size_t buflen) override;

private:
  detail::CaptureTask m_task;
  std::unique_ptr<Frame> m_frame;
  size_t m_index = 0;
};

/** @brief Handle HTTP request for still image. */
inline void
handleStill(AsyncWebServerRequest* request) {
  request->send(new StillResponse());
}

/**
 * @brief HTTP response of MJPEG stream.
 *
 * Start a FreeRTOS task to capture image frames under the current camera settings,
 * and respond to the HTTP request as a Motion JPEG stream.
 * If multiple MjpegResponse instances are active concurrently, each stream will contain
 * different images.
 * If task creation fails, respond with HTTP 500 error.
 * If image capture fails, the stream is stopped.
 *
 * Normally, a new frame is captured after the prior frame has been fully sent to the client.
 * Setting MjpegConfig::minInternal to -1 enables continuous mode, in which a new frame is
 * captured while the prior frame is still being sent to the client. This improves FPS rate
 * but also increases video latency. This mode is effective only if there are more than one
 * frame buffer created during camera initialization.
 */
class MjpegResponse : public AsyncAbstractResponse {
public:
  explicit MjpegResponse(const MjpegConfig& cfg = MjpegConfig());

  ~MjpegResponse() override;

  bool _sourceValid() const override {
    return true;
  }

  size_t _fillBuffer(uint8_t* buf, size_t buflen) override;

private:
  size_t sendPart(uint8_t* buf, size_t buflen);

private:
  detail::CaptureTask m_task;
  using Ctrl = detail::MjpegController;
  Ctrl m_ctrl;
  detail::MjpegHeader m_hdr;

  enum SendItem {
    SINone,
    SIPartHeader,
    SIFrame,
    SIPartTrailer,
  };
  SendItem m_sendNext = SINone;
  const uint8_t* m_sendBuf = nullptr;
  size_t m_sendRemain = 0;
  unsigned long m_createTime = millis();
  unsigned long m_sendSince = 0;
};

/** @brief Handle HTTP request for MJPEG stream. */
inline void
handleMjpeg(AsyncWebServerRequest* request) {
  request->send(new MjpegResponse());
}

} // namespace asyncweb
} // namespace esp32cam

#endif // __has_include(<ESPAsyncWebServer.h>)
#endif // ESP32CAM_ASYNCWEB_HPP
