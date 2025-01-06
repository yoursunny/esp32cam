#include "setters.hpp"
#include <esp_camera.h>

namespace esp32cam {

SensorSetterInt::SensorSetterInt(size_t fn, int value)
  : m_fn(fn)
  , m_value(value) {}

bool
SensorSetterInt::operator()(sensor_t* sensor) const {
  using Fn = int (*)(sensor_t*, int);
  Fn* fn = reinterpret_cast<Fn*>(reinterpret_cast<uintptr_t>(sensor) + m_fn);
  return (*fn)(sensor, m_value) == 0;
}

SetHmirror::SetHmirror(bool enable)
  : SensorSetterInt(offsetof(sensor_t, set_hmirror), static_cast<int>(enable)) {}

SetVflip::SetVflip(bool enable)
  : SensorSetterInt(offsetof(sensor_t, set_vflip), static_cast<int>(enable)) {}

} // namespace esp32cam
