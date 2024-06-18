#ifndef ESP32CAM_ASYNCWEB_H
#define ESP32CAM_ASYNCWEB_H

#include "esp32cam.h"

#include <ESPAsyncWebServer.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#ifdef ESP32CAM_ASYNCWEB_LOGGER
#define ESP32CAM_ASYNCWEB_LOG(fmt, ...)                                                            \
  ESP32CAM_ASYNCWEB_LOGGER.printf("[%8lu] " fmt "\n", millis(), ##__VA_ARGS__)
#else
#define ESP32CAM_ASYNCWEB_LOG(...)                                                                 \
  do {                                                                                             \
  } while (false)
#endif

#define ESP32CAM_ASYNCWEB_MJPEG_LOG(fmt, ...)                                                      \
  ESP32CAM_ASYNCWEB_LOG("MjpegResponse(%p) " fmt, this, ##__VA_ARGS__)

namespace esp32cam {
/**
 * @brief esp32cam integration with ESPAsyncWebServer library.
 * @sa https://github.com/me-no-dev/ESPAsyncWebServer
 */
namespace asyncweb {

/** @brief HTTP response of one still image. */
class StillResponse : public AsyncAbstractResponse {
public:
  /**
   * @brief Constructor.
   * @param frame a frame of still image.
   */
  explicit StillResponse(std::unique_ptr<Frame> frame)
    : m_frame(std::move(frame)) {
    _code = 200;
    _contentType = determineMineType(*m_frame);
    _contentLength = m_frame->size();
    _sendContentLength = true;
  }

  bool _sourceValid() const override {
    return true;
  }

  size_t _fillBuffer(uint8_t* buf, size_t buflen) override {
    if (m_index >= m_frame->size()) {
      return 0;
    }

    size_t len = std::min(buflen, m_frame->size() - m_index);
    std::copy_n(m_frame->data() + m_index, len, buf);
    m_index += len;
    return len;
  }

public:
  /**
   * @brief Capture a frame and respond to the request.
   * @param ctx AsyncWebServerRequest* pointer.
   *
   * This function must run as a FreeRTOS task. It self-deletes upon completion.
   */
  static void captureTask(void* ctx) {
    auto request = reinterpret_cast<AsyncWebServerRequest*>(ctx);

    auto frame = Camera.capture();
    if (frame == nullptr) {
      request->send(500);
    } else {
      request->send(new StillResponse(std::move(frame)));
    }

    vTaskDelete(nullptr);
  }

private:
  static const char* determineMineType(const Frame& frame) {
    if (frame.isJpeg()) {
      return "image/jpeg";
    }
    if (frame.isBmp()) {
      return "image/bmp";
    }
    return "application/octet-stream";
  }

private:
  std::unique_ptr<Frame> m_frame;
  size_t m_index = 0;
};

/**
 * @brief Handle HTTP request for still image.
 *
 * Start a FreeRTOS task to capture one frame of still image under the current camera settings,
 * and then respond to the HTTP request with the frame.
 * If this function is called multiple times with concurrent HTTP requests, each response will
 * contain a different image.
 * If task creation or image capture fails, respond with HTTP 500 error.
 *
 * To perform authentication or other operations before image capture, create another HTTP handler
 * to do these, and then call this function.
 */
inline void
handleStill(AsyncWebServerRequest* request) {
  TaskHandle_t task;
  auto res = xTaskCreatePinnedToCore(StillResponse::captureTask, "esp32cam-still", 2048, request, 1,
                                     &task, xPortGetCoreID());
  if (res != pdPASS) {
    request->send(500);
  }
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
 */
class MjpegResponse : public AsyncAbstractResponse {
public:
  explicit MjpegResponse(const MjpegConfig& cfg = MjpegConfig())
    : m_ctrl(cfg) {
    ESP32CAM_ASYNCWEB_MJPEG_LOG("created");
    m_queue = xQueueCreate(4, sizeof(Frame*));
    if (xTaskCreatePinnedToCore(captureTask, "esp32cam-mjpeg", 2048, this, 1, &m_task,
                                xPortGetCoreID()) != pdPASS) {
      m_task = nullptr;
    };
    if (m_queue == nullptr || m_task == nullptr) {
      _code = 500;
      m_ctrl.notifyFail();
      return;
    }

    _code = 200;
    m_hdr.prepareResponseContentType();
    _contentType = String(m_hdr.buf, m_hdr.size);
    _sendContentLength = false;
  }

  ~MjpegResponse() override {
    if (m_task != nullptr) {
      vTaskDelete(m_task);
      m_task = nullptr;
    }

    if (m_queue != nullptr) {
      Frame* frame = nullptr;
      while (xQueueReceive(m_queue, &frame, 0) == pdTRUE) {
        delete frame;
      }
      vQueueDelete(m_queue);
      m_queue = nullptr;
    }

    int nFrames = m_ctrl.countSentFrames();
    float fps = 1000.0 * nFrames / (millis() - m_createTime);
    ESP32CAM_ASYNCWEB_MJPEG_LOG("deleted after %d frames at fps %f", m_ctrl.countSentFrames(), fps);
  }

  bool _sourceValid() const override {
    return true;
  }

  size_t _fillBuffer(uint8_t* buf, size_t buflen) override {
    auto act = m_ctrl.decideAction();
    switch (act) {
      case Ctrl::RETURN: {
        Frame* frame = nullptr;
        if (xQueueReceive(m_queue, &frame, 0) == pdTRUE) {
          m_ctrl.notifyReturn(std::unique_ptr<Frame>(frame));
          ESP32CAM_ASYNCWEB_MJPEG_LOG("frame has %zu octets", frame->size());
        }
        m_sendNext = SIPartHeader;
        m_sendRemain = 0;

        if (m_ctrl.decideAction() != Ctrl::SEND) {
          return RESPONSE_TRY_AGAIN;
        }
        // fallthrough
      }
      case Ctrl::SEND: {
        size_t len = sendPart(buf, buflen);
        if (len == 0 && m_sendNext == SINone) {
          m_ctrl.notifySent(true);
          ESP32CAM_ASYNCWEB_MJPEG_LOG("sent to client");
        } else {
          return len;
        }

        if (m_ctrl.decideAction() != Ctrl::CAPTURE) {
          return RESPONSE_TRY_AGAIN;
        }
        // fallthrough
      }
      case Ctrl::CAPTURE: {
        xTaskNotify(m_task, 1, eSetValueWithOverwrite);
        m_ctrl.notifyCapture();
        ESP32CAM_ASYNCWEB_MJPEG_LOG("capturing");
        return RESPONSE_TRY_AGAIN;
      }
      case Ctrl::STOP:
        ESP32CAM_ASYNCWEB_MJPEG_LOG("stopping");
        return 0;
      default:
        return RESPONSE_TRY_AGAIN;
    }
  }

private:
  static void captureTask(void* ctx) {
    auto self = reinterpret_cast<MjpegResponse*>(ctx);
    while (true) {
      uint32_t value = 0;
      xTaskNotifyWait(0, UINT32_MAX, &value, pdMS_TO_TICKS(10000));
      if (value == 0) {
        continue;
      }

      auto frame = Camera.capture().release();
      while (xQueueSend(self->m_queue, &frame, pdMS_TO_TICKS(10000)) != pdTRUE) {
        ;
      }
    }
  }

  size_t sendPart(uint8_t* buf, size_t buflen) {
    if (m_sendRemain == 0) {
      switch (m_sendNext) {
        case SIPartHeader:
          m_hdr.preparePartHeader(m_ctrl.getFrame()->size());
          m_sendBuf = reinterpret_cast<const uint8_t*>(m_hdr.buf);
          m_sendRemain = m_hdr.size;
          m_sendNext = SIFrame;
          break;
        case SIFrame:
          m_sendBuf = m_ctrl.getFrame()->data();
          m_sendRemain = m_ctrl.getFrame()->size();
          m_sendNext = SIPartTrailer;
          break;
        case SIPartTrailer:
          m_hdr.preparePartTrailer();
          m_sendBuf = reinterpret_cast<const uint8_t*>(m_hdr.buf);
          m_sendRemain = m_hdr.size;
          m_sendNext = SINone;
          break;
        case SINone:
          return 0;
      }
    }

    size_t len = std::min(m_sendRemain, buflen);
    std::copy_n(m_sendBuf, len, buf);
    m_sendBuf += len;
    m_sendRemain -= len;
    return len;
  }

private:
  QueueHandle_t m_queue = nullptr;
  TaskHandle_t m_task = nullptr;

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
};

/**
 * @brief Handle HTTP request for MJPEG stream.
 *
 * To specify MjpegConfig, construct MjpegResponse directly.
 */
inline void
handleMjpeg(AsyncWebServerRequest* request) {
  request->send(new MjpegResponse());
}

} // namespace asyncweb
} // namespace esp32cam

#endif // ESP32CAM_ASYNCWEB_H
