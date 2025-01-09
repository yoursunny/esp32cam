#include "mjpeg.hpp"
#include "logger.hpp"

#define MC_LOG(fmt, ...) ESP32CAM_LOG("MjpegController(%p) " fmt, this, ##__VA_ARGS__)

namespace esp32cam {
namespace detail {

MjpegController::MjpegController(MjpegConfig cfg)
  : cfg(cfg)
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
  m_nextCaptureTime = millis() + static_cast<unsigned long>(std::max(0, cfg.minInterval));
  MC_LOG("notifyCapture next=%lu", m_nextCaptureTime);
}

void
MjpegController::notifyReturn(std::unique_ptr<Frame> frame) {
  if (frame == nullptr) {
    MC_LOG("notifyReturn frame=nullptr");
    notifyFail();
    return;
  }
  m_frame = std::move(frame);
  MC_LOG("notifyReturn frame=%p size=%zu dimension=%dx%d", m_frame->data(), m_frame->size(),
         m_frame->getWidth(), m_frame->getHeight());
  m_nextAction = SEND;
}

void
MjpegController::notifySent(bool ok) {
  ++m_count;
  MC_LOG("notifySent count=%d ok=%d", m_count, static_cast<int>(ok));
  if (!ok) {
    notifyFail();
    return;
  }
  m_frame.reset();
  m_nextAction = cfg.maxFrames < 0 || m_count < cfg.maxFrames ? CAPTURE : STOP;
}

void
MjpegController::notifyFail() {
  MC_LOG("notifyFail");
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
