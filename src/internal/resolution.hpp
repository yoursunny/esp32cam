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
  bool
  isValid() const;

  int
  getWidth() const;

  int
  getHeight() const;

  /** \brief Find a resolution that satisfies given constraints.
   */
  static Resolution
  find(int minWidth, int minHeight);

private:
  int m_frameSize = -1;

  friend class Config;
};

} // namespace esp32cam

#endif // ESP32CAM_RESOLUTION_HPP
