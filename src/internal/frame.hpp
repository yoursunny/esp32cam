#ifndef ESP32CAM_FRAME_HPP
#define ESP32CAM_FRAME_HPP

#include <cstddef>
#include <cstdint>

class Client;
class Print;

#if defined(ESP8266)
# include <ESP8266WiFi.h>
# include <ESPAsyncTCP.h>
#endif
#if defined(ESP32)
# include "freertos/FreeRTOS.h"
# include <AsyncTCP.h>
#endif

namespace esp32cam {

/** \brief A frame of picture.
 */
class Frame
{
public: // access
  ~Frame();

  uint8_t* data() const
  {
    return m_data;
  }

  size_t size() const
  {
    return m_size;
  }

  int getWidth() const
  {
    return m_width;
  }

  int getHeight() const
  {
    return m_height;
  }

  /** \brief Write frame buffer to \p os .
   *  \param os output stream.
   *  \param timeout total time limit in millis.
   *  \retval true writing completed.
   *  \retval false writing disrupted by timeout.
   */
  bool 
  writeTo(Print& os, int timeout = 10000);

  /** \brief Write frame buffer to \p os .
   *  \param os output socket.
   *  \param timeout total time limit in millis.
   *  \retval true writing completed.
   *  \retval false writing disrupted by timeout or socket error.
   */
  bool writeTo(Client& os, int timeout = 10000);

  /** \brief Write frame buffer to \p os .
   *  \param os output socket.
   *  \param timeout total time limit in millis.
   *  \retval true writing completed.
   *  \retval false writing disrupted by timeout or socket error.
   */
  bool 
  writeTo(AsyncClient& os, int timeout = 10000);

public: // conversion
  bool isJpeg() const;

  /** \brief Convert frame to JPEG.
   *  \param quality JPEG quality between 0 (worst) and 100 (best).
   */
  bool toJpeg(int quality);

  bool isBmp() const
  {
    return m_pixFormat == PIXFORMAT_BMP;
  }

  int 
  getPixFormat() 
  {
    return m_pixFormat;
  }

  /** \brief Convert frame to BMP.
   */
  bool toBmp();
  
  /** \brief Assign buffer from outside 
   *  \We converted the buffer ourself outside the lib and need to set it!
   */
  void 
  setData(uint8_t* newdata, size_t newsize, bool bIsBmp);

private:
  Frame();

  explicit Frame(void* fb);

  bool writeToImpl(Print& os, int timeout, Client* client);

  bool writeToImpl(int timeout, AsyncClient* client); 

  void releaseFb();

private:
  class CameraFbT; ///< camera_fb_t
  CameraFbT* m_fb = nullptr;
  uint8_t* m_data = nullptr;
  size_t m_size = 0;
  int m_width = -1;
  int m_height = -1;

  enum
  {
    PIXFORMAT_BMP = -101,
  };
  int m_pixFormat = -1;

  friend class CameraClass;
};

} // namespace esp32cam

#endif // ESP32CAM_FRAME_HPP
