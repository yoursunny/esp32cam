#include "config.hpp"

#include <esp_camera.h>

namespace esp32cam {

bool
Resolution::isValid() const
{
  return m_frameSize >= 0 && m_frameSize < FRAMESIZE_INVALID;
}

int
Resolution::getWidth() const
{
  if (!isValid()) {
    return -1;
  }
  return ::resolution[m_frameSize][0];
}

int
Resolution::getHeight() const
{
  if (!isValid()) {
    return -1;
  }
  return ::resolution[m_frameSize][1];
}

Resolution
Resolution::find(int minWidth, int minHeight)
{
  Resolution res;
  for (res.m_frameSize = 0; res.m_frameSize < FRAMESIZE_INVALID; ++res.m_frameSize) {
    if (res.getWidth() >= minWidth && res.getHeight() >= minHeight) {
      break;
    }
  }
  return res;
}

} // namespace esp32cam
