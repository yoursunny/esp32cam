#ifndef ESP32CAM_SETTERS_HPP
#define ESP32CAM_SETTERS_HPP

#include <cstddef>

extern "C" {
typedef struct _sensor sensor_t;
} // extern "C"

namespace esp32cam {

class SensorSetter {
public:
  virtual ~SensorSetter() {}

  virtual bool operator()(sensor_t* sensor) const = 0;
};

class SensorSetterInt : public SensorSetter {
public:
  bool operator()(sensor_t* sensor) const override;

protected:
  explicit SensorSetterInt(size_t fn, int value);

private:
  size_t m_fn;
  int m_value;
};

/**
 * @brief Enable or disable horizontal flip.
 * @sa Camera.update()
 */
class SetHmirror : public SensorSetterInt {
public:
  explicit SetHmirror(bool enable);
};

/**
 * @brief Enable or disable vertical flip.
 * @sa Camera.update()
 */
class SetVflip : public SensorSetterInt {
public:
  explicit SetVflip(bool enable);
};

} // namespace esp32cam

#endif // ESP32CAM_SETTERS_HPP
