#ifndef ESP32CAM_ASYNCWEB_H
#define ESP32CAM_ASYNCWEB_H

#include "esp32cam.h"

#include <ESPAsyncWebServer.h>
#include <freertos/task.h>

namespace esp32cam {
/**
 * @brief esp32cam integration with ESPAsyncWebServer library.
 * @sa https://github.com/me-no-dev/ESPAsyncWebServer
 */
namespace asyncweb {

/** @brief HTTP response containing one still image. */
class StillResponse : public AsyncCallbackResponse
{
public:
  /**
   * @brief Constructor.
   * @param frame a frame of still image.
   */
  explicit StillResponse(std::unique_ptr<Frame> frame)
    : AsyncCallbackResponse(determineMineType(*frame), frame->size(),
                            [this](uint8_t* buffer, size_t maxLen, size_t index) {
                              return fill(buffer, maxLen, index);
                            })
    , m_frame(std::move(frame))
  {}

private:
  static const char* determineMineType(const Frame& frame)
  {
    if (frame.isJpeg()) {
      return "image/jpeg";
    }
    if (frame.isBmp()) {
      return "image/bmp";
    }
    return "application/octet-stream";
  }

  size_t fill(uint8_t* buffer, size_t maxLen, size_t index)
  {
    if (index >= m_frame->size()) {
      return 0;
    }

    size_t len = std::min(maxLen, m_frame->size() - index);
    std::copy_n(m_frame->data() + index, len, buffer);
    return len;
  }

private:
  std::unique_ptr<Frame> m_frame;
};

namespace detail {

inline void
captureStill(void* ctx)
{
  auto request = reinterpret_cast<AsyncWebServerRequest*>(ctx);

  auto frame = Camera.capture();
  if (frame == nullptr) {
    request->send(500);
  } else {
    request->send(new StillResponse(std::move(frame)));
  }

  vTaskDelete(xTaskGetCurrentTaskHandle());
}

} // namespace detail

/**
 * @brief Handle HTTP request for still image capture.
 *
 * Start a FreeRTOS task to capture one frame of still image under the current camera settings,
 * and then respond to the HTTP request with the frame.
 * If this function is called multiple times with concurrent HTTP requests, each response will
 * contain a different image.
 * If task creation or image capture fails, respond with HTTP 500 error.
 */
inline void
handleStill(AsyncWebServerRequest* request)
{
  TaskHandle_t task;
  auto res =
    xTaskCreatePinnedToCore(detail::captureStill, "esp32cam", CONFIG_ARDUINO_LOOP_STACK_SIZE,
                            request, 1, &task, xPortGetCoreID());
  if (res != pdPASS) {
    request->send(500);
  }
}

} // namespace asyncweb
} // namespace esp32cam

#endif // ESP32CAM_ASYNCWEB_H
