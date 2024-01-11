#include "mjpeg.hpp"

#include <Arduino.h>

namespace esp32cam {
namespace detail {

MjpegController::MjpegController(MjpegConfig cfg)
  : m_cfg(cfg)
  , m_nextCaptureTime(millis()) {}

int
MjpegController::decideAction() {
  if (m_nextAction == CAPTURE) {
    auto t = static_cast<int>(m_nextCaptureTime - millis());
    if (t > 0) {
      return t;
    }
    return CAPTURE;
  }
  return m_nextAction;
}

void
MjpegController::notifyCapture() {
  m_nextAction = RETURN;
  m_nextCaptureTime = millis() + static_cast<unsigned long>(m_cfg.minInterval);
}

void
MjpegController::notifyReturn(std::unique_ptr<Frame> frame) {
  if (frame == nullptr) {
    notifyFail();
    return;
  }
  m_frame = std::move(frame);
  m_nextAction = SEND;
}

void
MjpegController::notifySent(bool ok) {
  ++m_count;
  if (!ok) {
    notifyFail();
    return;
  }
  m_frame.reset();
  m_nextAction = m_cfg.maxFrames < 0 || m_count < m_cfg.maxFrames ? CAPTURE : STOP;
}

void
MjpegController::notifyFail() {
  m_frame.reset();
  m_nextAction = STOP;
}

#define BOUNDARY "e8b8c539-047d-4777-a985-fbba6edff11e"

void
MjpegHeader::prepareResponseHeaders() {
  size = snprintf(buf, sizeof(buf),
                  "HTTP/1.1 200 OK\r\n"
                  "Content-Type: multipart/x-mixed-replace;boundary=" BOUNDARY "\r\n"
                  "\r\n");
}

void
MjpegHeader::prepareResponseContentType() {
  size = snprintf(buf, sizeof(buf), "multipart/x-mixed-replace;boundary=" BOUNDARY);
}

void
MjpegHeader::preparePartHeader(size_t contentLength) {
  size = snprintf(buf, sizeof(buf),
                  "Content-Type: image/jpeg\r\n"
                  "Content-Length: %zu\r\n"
                  "\r\n",
                  contentLength);
}

void
MjpegHeader::preparePartTrailer() {
  size = snprintf(buf, sizeof(buf), "\r\n--" BOUNDARY "\r\n");
}

size_t
MjpegHeader::writeTo(Print& os) {
  return os.write(buf, size);
}

} // namespace detail
} // namespace esp32cam
