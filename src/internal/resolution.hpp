#ifndef ESP32CAM_RESOLUTION_HPP
#define ESP32CAM_RESOLUTION_HPP

#include <Printable.h>
#include <iterator>

namespace esp32cam {

class ResolutionList;

/** @brief Picture width and height. */
class Resolution : public Printable
{
public:
  /** @brief Return an iterable collection of possible resolutions. */
  static ResolutionList list();

  /** @brief Find a resolution that satisfies given constraints. */
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
  T as() const
  {
    return static_cast<T>(m_frameSize);
  }

private:
  int m_frameSize;

  friend class ResolutionList;

  friend bool operator==(const Resolution& lhs, const Resolution& rhs)
  {
    return (!lhs.isValid() && !rhs.isValid()) || (lhs.m_frameSize == rhs.m_frameSize);
  }

  friend bool operator!=(const Resolution& lhs, const Resolution& rhs)
  {
    return !(lhs == rhs);
  }
};

/**
 * @brief A collection of possible resolutions.
 * @code
 * for (const auto& resolution : Resolution::list()) {
 *   Serial.println(resolution);
 * }
 * @endcode
 */
class ResolutionList
{
public:
  class Iterator
  {
  public:
    using iterator_catagory = std::forward_iterator_tag;
    using value_type = const Resolution;
    using difference_type = std::ptrdiff_t;
    using pointer = void;
    using reference = value_type;

    Iterator() = default;

    explicit Iterator(Resolution value)
      : m_value(value)
    {}

    Iterator& operator++()
    {
      ++m_value.m_frameSize;
      return *this;
    }

    Iterator operator++(int)
    {
      Iterator copy(*this);
      ++*this;
      return copy;
    }

    reference operator*()
    {
      return m_value;
    }

  private:
    Resolution m_value;

    friend bool operator==(const Iterator& lhs, const Iterator& rhs)
    {
      return lhs.m_value == rhs.m_value;
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs)
    {
      return !(lhs == rhs);
    }
  };

  Iterator begin() const;

  Iterator end() const;
};

} // namespace esp32cam

#endif // ESP32CAM_RESOLUTION_HPP
