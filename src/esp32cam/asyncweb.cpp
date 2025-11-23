#if __has_include(<ESPAsyncWebServer.h>)

#include "asyncweb.hpp"
#include "logger.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/idf_additions.h>

#define CAPTURE_LOG(fmt, ...) ESP32CAM_LOG("CaptureTask(%p) " fmt, this, ##__VA_ARGS__)
#define STILL_LOG(fmt, ...) ESP32CAM_LOG("StillResponse(%p) " fmt, this, ##__VA_ARGS__)
#define MJPEG_LOG(fmt, ...) ESP32CAM_LOG("MjpegResponse(%p) " fmt, this, ##__VA_ARGS__)

namespace esp32cam {
namespace detail {

CaptureTask::CaptureTask(uint32_t queueLength, uint32_t priority, bool continuous)
  : m_continuous(continuous) {
  m_queue = xQueueCreate(queueLength, sizeof(Frame*));
  if (m_queue == nullptr) {
    CAPTURE_LOG("queue create fail");
    return;
  }

  TaskHandle_t task = nullptr;
  if (xTaskCreatePinnedToCore(run0, "esp32cam-capture", 2048, this, priority, &task,
                              xPortGetCoreID()) != pdPASS) {
    CAPTURE_LOG("task create fail");
    return;
  }

  m_task = task;
  CAPTURE_LOG("created");
}

CaptureTask::~CaptureTask() {
  if (m_task != nullptr) {
    vTaskDelete(reinterpret_cast<TaskHandle_t>(m_task));
    m_task = nullptr;
  }

  if (m_queue != nullptr) {
    Frame* frame = nullptr;
    while (xQueueReceive(reinterpret_cast<QueueHandle_t>(m_queue), &frame, 0) == pdTRUE) {
      if (frame != nullptr) {
        CAPTURE_LOG("dequeued and deleted frame=%p", frame->data());
        delete frame;
      }
    }
    vQueueDelete(reinterpret_cast<QueueHandle_t>(m_queue));
    m_queue = nullptr;
  }

  CAPTURE_LOG("deleted");
}

void
CaptureTask::request() {
  if (m_task == nullptr || m_continuous) {
    return;
  }
  auto res = xTaskNotify(reinterpret_cast<TaskHandle_t>(m_task), 1, eSetValueWithoutOverwrite);
  CAPTURE_LOG("requested res=%d", static_cast<int>(res));
}

std::unique_ptr<Frame>
CaptureTask::retrieve() {
  Frame* frame = nullptr;
  if (m_queue == nullptr ||
      xQueueReceive(reinterpret_cast<QueueHandle_t>(m_queue), &frame, 0) != pdTRUE) {
    return nullptr;
  }
  CAPTURE_LOG("dequeued frame=%p", frame == nullptr ? nullptr : frame->data());
  return std::unique_ptr<Frame>(frame);
}

void
CaptureTask::run0(void* ctx) {
  auto self = reinterpret_cast<CaptureTask*>(ctx);
  self->run();
}

void
CaptureTask::run() {
  auto queue = reinterpret_cast<QueueHandle_t>(m_queue);
  while (true) {
    if (!m_continuous) {
      uint32_t value = 0;
      xTaskNotifyWait(0, UINT32_MAX, &value, pdMS_TO_TICKS(10000));
      if (value == 0) {
        continue;
      }
    }

    auto frame = Camera.capture().release();
    CAPTURE_LOG("captured frame=%p", frame == nullptr ? nullptr : frame->data());
    while (xQueueSend(queue, &frame, pdMS_TO_TICKS(10000)) != pdTRUE) {
      CAPTURE_LOG("enqueue fail room=%d", static_cast<int>(uxQueueSpacesAvailable(queue)));
    }
    CAPTURE_LOG("enqueued frame=%p", frame == nullptr ? nullptr : frame->data());
  }
}

} // namespace detail
namespace asyncweb {

StillResponse::StillResponse()
  : m_task(1) {
  STILL_LOG("created");
  if (!m_task) {
    _code = 500;
    return;
  }

  _code = 200;
  _contentType = "image/jpeg";
  _sendContentLength = false;
  m_task.request();
}

StillResponse::~StillResponse() {
  STILL_LOG("deleted");
}

size_t
StillResponse::_fillBuffer(uint8_t* buf, size_t buflen) {
  if (!m_frame) {
    if (m_frame = m_task.retrieve(); !m_frame) {
      return RESPONSE_TRY_AGAIN;
    }
    STILL_LOG("frame has %zu octets", m_frame->size());
  }

  if (m_index >= m_frame->size()) {
    STILL_LOG("sent to client");
    return 0;
  }

  size_t len = std::min(buflen, m_frame->size() - m_index);
  std::copy_n(m_frame->data() + m_index, len, buf);
  m_index += len;
  return len;
}

MjpegResponse::MjpegResponse(const MjpegConfig& cfg)
  : m_task(2, 1, cfg.minInterval < 0)
  , m_ctrl(cfg) {
  MJPEG_LOG("created");
  if (!m_task) {
    _code = 500;
    m_ctrl.notifyFail();
    return;
  }

  _code = 200;
  m_hdr.prepareResponseContentType();
  _contentType = String(m_hdr.buf, m_hdr.size);
  _sendContentLength = false;
}

MjpegResponse::~MjpegResponse() {
  int nFrames = m_ctrl.countSentFrames();
  float fps = 1000.0 * nFrames / (millis() - m_createTime);
  MJPEG_LOG("deleted after %d frames at fps %f", m_ctrl.countSentFrames(), fps);
}

size_t
MjpegResponse::_fillBuffer(uint8_t* buf, size_t buflen) {
  auto act = m_ctrl.decideAction();
  switch (act) {
    case Ctrl::RETURN: {
      if (auto frame = m_task.retrieve(); frame) {
        m_ctrl.notifyReturn(std::move(frame));
      }
      m_sendSince = millis();
      m_sendNext = SIBoundary;
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
      } else {
        if (static_cast<int>(millis() - m_sendSince) > m_ctrl.cfg.frameTimeout) {
          m_ctrl.notifySent(false);
        }
        return len;
      }

      if (m_ctrl.decideAction() != Ctrl::CAPTURE) {
        return RESPONSE_TRY_AGAIN;
      }
      // fallthrough
    }
    case Ctrl::CAPTURE: {
      m_task.request();
      m_ctrl.notifyCapture();
      return RESPONSE_TRY_AGAIN;
    }
    case Ctrl::STOP:
      return 0;
    default:
      return RESPONSE_TRY_AGAIN;
  }
}

size_t
MjpegResponse::sendPart(uint8_t* buf, size_t buflen) {
  if (m_sendRemain == 0) {
    switch (m_sendNext) {
      case SIBoundary:
        m_hdr.prepareBoundary(m_ctrl.getFrame()->size());
        m_sendBuf = reinterpret_cast<const uint8_t*>(m_hdr.buf);
        m_sendRemain = m_hdr.size;
        m_sendNext = SIFrame;
        break;
      case SIFrame: {
        Frame* frame = m_ctrl.getFrame();
        m_sendBuf = frame->data();
        m_sendRemain = frame->size();
        m_sendNext = SINone;
        break;
      }
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

} // namespace asyncweb
} // namespace esp32cam

#endif // __has_include(<ESPAsyncWebServer.h>)
