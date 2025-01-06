#ifndef ESP32CAM_FRAME_HPP
#define ESP32CAM_FRAME_HPP

#include <cstddef>
#include <cstdint>

class Client;
class Print;

namespace esp32cam {

/** @brief A frame of picture. */
class Frame {
public: // access
  ~Frame();

  uint8_t* data() const {
    return m_data;
  }

  size_t size() const {
    return m_size;
  }

  int getWidth() const {
    return m_width;
  }

  int getHeight() const {
    return m_height;
  }

  /**
   * @brief Write frame buffer to @p os .
   * @param os output stream.
   * @param timeout total time limit in millis.
   * @retval true writing completed.
   * @retval false writing disrupted by timeout.
   */
  bool writeTo(Print& os, int timeout = 10000);

  /**
   * @brief Write frame buffer to @p os .
   * @param os output socket.
   * @param timeout total time limit in millis.
   * @retval true writing completed.
   * @retval false writing disrupted by timeout or socket error.
   */
  bool writeTo(Client& os, int timeout = 10000);

public: // conversion
  bool isJpeg() const;

  /**
   * @brief Convert frame to JPEG.
   * @param quality JPEG quality between 0 (worst) and 100 (best).
   */
  bool toJpeg(int quality);

  bool isBmp() const;

  /** @brief Convert frame to BMP. */
  bool toBmp();

private:
  Frame();

  explicit Frame(void* fb);

  bool writeToImpl(Print& os, int timeout, Client* client);

  void releaseFb();

private:
  class CameraFbT; ///< camera_fb_t
  CameraFbT* m_fb = nullptr;
  uint8_t* m_data = nullptr;
  size_t m_size = 0;
  int m_width = -1;
  int m_height = -1;
  int m_pixFormat = -1;

  friend class CameraClass;
};

} // namespace esp32cam

#endif // ESP32CAM_FRAME_HPP
