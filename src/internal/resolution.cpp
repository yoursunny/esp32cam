#include "resolution.hpp"
#include <Print.h>

#include <esp_camera.h>

namespace esp32cam {

ResolutionList::Iterator
ResolutionList::begin() const
{
  return Iterator(Resolution(0));
}

ResolutionList::Iterator
ResolutionList::end() const
{
  return Iterator(Resolution());
}

ResolutionList
Resolution::list()
{
  static ResolutionList list;
  return list;
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

Resolution::Resolution(int frameSize)
  : m_frameSize(frameSize)
{}

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
  return ::resolution[m_frameSize].width;
}

int
Resolution::getHeight() const
{
  if (!isValid()) {
    return -1;
  }
  return ::resolution[m_frameSize].height;
}

size_t
Resolution::printTo(Print& p) const
{
  size_t len = 0;
  len += p.print(getWidth());
  len += p.print('x');
  len += p.print(getHeight());
  return len;
}

} // namespace esp32cam
