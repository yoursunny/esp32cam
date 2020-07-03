#ifndef ESP32CAM_RESOLUTION_HPP
#define ESP32CAM_RESOLUTION_HPP

namespace esp32cam {

/** \brief Picture width and height.
 */
class Resolution
{
public:
  /** \brief Determine if this resolution is valid.
   */
  bool isValid() const;

  int getWidth() const;

  int getHeight() const;

  /** \brief Find a resolution that satisfies given constraints.
   */
  static Resolution find(int minWidth, int minHeight);

  /** \brief Convert to framesize_t (internal use).
   *  \tparam T framesize_t
   */
  template<typename T>
  T as() const
  {
    return static_cast<T>(m_frameSize);
  }

private:
  int m_frameSize = -1;
};

} // namespace esp32cam

#endif // ESP32CAM_RESOLUTION_HPP
