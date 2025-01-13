#ifndef ESP32CAM_CONFIG_HPP
#define ESP32CAM_CONFIG_HPP

#include "pins.hpp"
#include "resolution.hpp"

namespace esp32cam {
namespace detail {

/** @brief Convert JPEG quality from 0-100 scale to 63-0 scale. */
int
convertJpegQuality(int quality);

} // namespace detail

/** @brief Camera initialization configuration. */
class Config {
public:
  Config();

  ~Config();

  Config& setPins(const Pins& pins);

  Config& setResolution(const Resolution& resolution);

  /**
   * @brief Set number of frame buffers.
   * @param n >=1, having more frame buffers allows better streaming fps.
   */
  Config& setBufferCount(int n);

  /** @brief Change pixel format to RGB565. */
  Config& setRgb();

  /** @brief Change pixel format to YUV422. */
  Config& setYuv();

  /** @brief Change pixel format to grayscale. */
  Config& setGrayscale();

  /**
   * @brief Change pixel format to JPEG.
   * @param quality JPEG quality between 0 (worst) and 100 (best).
   */
  Config& setJpeg(int quality);

private:
  class CameraConfigT; ///< camera_config_t
  CameraConfigT* m_cfg;

  friend class CameraClass;
};

/** @brief Light mode / white balance values. */
enum class LightMode {
  NONE = -1,
  AUTO = 0,
  SUNNY = 1,
  CLOUDY = 2,
  OFFICE = 3,
  HOME = 4,
};

/** @brief Special effect values. */
enum class SpecialEffect {
  NONE = 0,
  NEGATIVE = 1,
  BLACKWHITE = 2,
  REDDISH = 3,
  GREENISH = 4,
  BLUISH = 5,
  ANTIQUE = 6,
};

/** @brief Camera runtime settings. */
struct Settings {
  /** @brief Picture resolution. */
  Resolution resolution;

  /** @brief Image brightness, between -2 and +2. */
  int8_t brightness;

  /** @brief Image contrast, between -2 and +2. */
  int8_t contrast;

  /** @brief Image saturation, between -2 and +2. */
  int8_t saturation;

  /**
   * @brief Gain control, with or without Automatic Gain Control (AGC).
   *
   * - AGC disabled: a positive number between 1 and 31, which corresponds to 1x ~ 31x gain.
   * - AGC enabled: -2,-4,-8,-16,-32,-64,-128, which corresponds to 2x ~ 128x gain.
   */
  int8_t gain;

  /** @brief Image light mode. */
  LightMode lightMode;

  /** @brief Image special effect. */
  SpecialEffect specialEffect;

  /** @brief Horizontal flip. */
  bool hmirror = false;

  /** @brief Vertical flip. */
  bool vflip = false;

  /** @brief Raw gamma mode. */
  bool rawGma = false;

  /** @brief Lens correction mode. */
  bool lensCorrection = false;
};

} // namespace esp32cam

#endif // ESP32CAM_CONFIG_HPP
