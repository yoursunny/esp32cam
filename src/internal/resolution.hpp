#ifndef ESP32CAM_RESOLUTION_HPP
#define ESP32CAM_RESOLUTION_HPP

#include <Printable.h>
#include <iterator>

namespace esp32cam {

class ResolutionList;

/** @brief Picture width and height. */
class Resolution : public Printable {
public:
  /**
   * @brief Return an iterable collection of possible resolutions.
   *
   * This list contains all resolutions defined in the esp32-camera library,
   * but not necessarily supported by the camera hardware.
   * Use @c Camera.listResolutions() to retrieve supported resolutions.
   */
  static ResolutionList list();

  /**
   * @brief Find a resolution that satisfies given constraints.
   *
   * This searches among all resolutions defined in the esp32-camera libary,
   * which is not necessarily supported by the camera hardware.
   */
  static Resolution find(int minWidth, int minHeight);

  /**
   * @brief Constructor.
   * @param frameSize framesize_t value.
   */
  explicit Resolution(int frameSize = -1);

  /** @brief Determine if this resolution is valid. */
  bool isValid() const;

  /** @brief Return picture width in pixels. */
  int getWidth() const;

  /** @brief Return picture height in pixels. */
  int getHeight() const;

  /** @brief Print WxH to output stream. */
  size_t printTo(Print& p) const override;

  /**
   * @brief Convert to framesize_t (internal use).
   * @tparam T framesize_t
   */
  template<typename T>
  T as() const {
    return static_cast<T>(m_frameSize);
  }

private:
  int m_frameSize; ///< framesize_t

  friend bool operator==(const Resolution& lhs, const Resolution& rhs) {
    return (!lhs.isValid() && !rhs.isValid()) || (lhs.m_frameSize == rhs.m_frameSize);
  }

  friend bool operator!=(const Resolution& lhs, const Resolution& rhs) {
    return !(lhs == rhs);
  }

  friend bool operator<(const Resolution& lhs, const Resolution& rhs) {
    return lhs.m_frameSize < rhs.m_frameSize;
  }

  friend bool operator>(const Resolution& lhs, const Resolution& rhs) {
    return lhs.m_frameSize > rhs.m_frameSize;
  }
};

/**
 * @brief A collection of resolutions.
 * @code
 * for (const auto& resolution : Resolution::list()) {
 *   Serial.println(resolution);
 * }
 * @endcode
 */
class ResolutionList {
public:
  class Iterator {
  public:
    using iterator_catagory = std::forward_iterator_tag;
    using value_type = const Resolution;
    using difference_type = std::ptrdiff_t;
    using pointer = void;
    using reference = value_type;

    explicit Iterator(int value = -1)
      : m_frameSize(value) {}

    Iterator& operator++() {
      ++m_frameSize;
      return *this;
    }

    Iterator operator++(int) {
      Iterator copy(*this);
      ++*this;
      return copy;
    }

    reference operator*() {
      return Resolution(m_frameSize);
    }

  private:
    int m_frameSize; ///< framesize_t

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
      return lhs.m_frameSize == rhs.m_frameSize;
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
      return !(lhs == rhs);
    }
  };

  /**
   * @brief Constructor.
   * @param max exclusive maximum framesize_t.
   */
  explicit ResolutionList(int max = 0);

  Iterator begin() const;

  Iterator end() const;

  /** @brief Find a resolution that satisfies given constraints. */
  Resolution find(int minWidth, int minHeight) const;

private:
  int m_max;
};

} // namespace esp32cam

#endif // ESP32CAM_RESOLUTION_HPP
