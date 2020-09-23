#ifndef ESP32CAM_H
#define ESP32CAM_H

#include "internal/config.hpp"
#include "internal/frame.hpp"

#include <memory>

namespace esp32cam {

struct StreamMjpegConfig
{
/// minimum interval between frame captures.
int minInterval = 0;
/// maximum number of frames before disconnecting.
int maxFrames = -1;
/// time limit of writing one frame in millis.
int frameTimeout = 10000;
};

class CameraClass
{
public:
  /** \brief Enable camera.
   */
  bool
  begin(const Config& config);

  /** \brief Disable camera.
   */
  bool
  end();

  /** \brief Change camera resolution.
   *  \pre Initial resolution is higher than specified resolution.
   *  \param resolution new resolution
   *  \param sleepFor how long to wait for stabilization (millis)
   */
  bool
  changeResolution(const Resolution& resolution, int sleepFor = 500);

  /** \brief Change camera resolution.
   *  \pre Change Camera Frequence Clock
   *  \param iMhz new Frequence, must be in MHZ
   */
  bool
  changeFrequence(int iMhz);

  /** \brief Change Sensor Contrast level
  * must be -2 to +2, default 0
  */
  bool
  changeContrast(int ilevel);

  /** \brief Change Brightness level
   *  \param ilevel must be -2 to +2, default 0
   */
  bool
  changeBrightness(int ilevel);

  /** \brief Change Saturation level
   *  \param ilevel must be -2 to +2, default 0
   */
  bool 
  changeSaturation(int ilevel);

  /** \brief Change SpecialEffect
   *  \param ilevel must be > 0 && <= NUM_SPECIAL_EFFECTS, default 0
   *  \	1 = no effect
   *  \ 2 = negative
   *  \ 3 = black and white
   *  \ 4 = reddish
   *  \ 5 = greenish
   *  \ 6 = blue
   *  \ 7 = retro
   */
  bool
  changeSpecialEffect(int ilevel);

  /** \brief Change WbMode
   *  \param ilevel must be > 0 && <= NUM_WB_MODES, default 0
   *  \ 0 = default
   *  \ 1 = sunny
   *  \ 2 = cloudy
   *  \ 3 = office
   *  \ 4 = home
   */
  bool
  changeWbMode(int ilevel);

  /** \brief Change AE Levels
   *  \param ilevel must be -2 to +2, default 0
   */
  bool
  changeAeLevels(int ilevel);
  
  /** \brief Enable/Disable  AEC (Auto Exposure Control)
   *  \param iEnable must be 0(disable) or 1 (enable)
   */
  bool
  changAutoExposurecontrol(int iEnable);

  /** \brief Change AGC Gain
   *  \param ilevel must be >= 0 and <= 30, default 30
   */
  bool
  changAgcGain(int ilevel);

  /** \brief Change Gainceiling
   *  \param iGainCeiling must be between GAINCEILING_2X and GAINCEILING_128X
   */
  bool
  changGainceilingSensor(int iGainCeiling);

  /** \brief Enable/Disable Awb GainControl
   *  \param iEnable must be 0(disable) or 1 (enable)
   */
  bool
  changeAwbGainControl(int iEnable);
  
  /** \brief Enable/Disable Gaincontrol
   *  \param iEnable must be 0(disable) or 1 (enable)
   */
  bool
  changeGaincontrol(int iEnable);

  /** \brief Enable/Disable Testing Colorbar
   *  \param iEnable must be 0(disable) or 1 (enable)
   */
  bool
  changeColorbar(int iEnable);

  /** \brief Enable/Disable  Whitebalance
   *  \param iEnable must be 0(disable) or 1 (enable)
   */
  bool
  changeWhitebalance(int iEnable);

  /** \brief Change JPEG Quality 
   *  \param iEnable iQuality must be within 0 and 63
   */
  bool changeQuality(int iQuality);	

  /** \brief Change AEC Value
   *  \param iEnable iValue must be 0 to 1200, default 0
   */
  bool
  changeAecValue(int iValue);

  /** \brief Enable/Disable Auto/Manual Exposure control
   *  \param iEnable must be 0(disable) or 1 (enable)
  */
  bool
  changAec2(int iEnable);

  /** \brief Enable/Disable horizontal mirror
   *  \param iEnable must be 0(disable) or 1 (enable)
  */
  bool
  changHMirror(int iEnable);

  /** \brief Enable/Disable vertical mirror
   *  \param iEnable must be 0(disable) or 1 (enable)
  */
  bool
  changVFlip(int iEnable);

  /** \brief Enable/Disable black pixel correction
   *  \param iEnable must be 0(disable) or 1 (enable)
  */
  bool
  changBPC(int iEnable);

  /** \brief Enable/Disable white pixel correction
   *  \param iEnable must be 0(disable) or 1 (enable)
  */
  bool
  changWPC(int iEnable);

  /** \brief Enable/Disable Denoise Control
   *  \param iEnable must be 0(disable) or 1 (enable)
  */
  bool
  changDenoise(int iEnable);

  /** \brief Enable/Disable Lenc Correction
   *  \param iEnable must be 0(disable) or 1 (enable)
  */
  bool
  changLenc(int iEnable);

  /** \brief Enable/Disable RAW Gamma
   *  \param iEnable must be 0(disable) or 1 (enable)
  */
  bool
  changRawGMA(int iEnable);
  
  /** \brief Enable/Disable DCW
   *  \param iEnable must be 0(disable) or 1 (enable)
  */
  bool
  changDcw(int iEnable);
  
  /** \brief Capture a frame of picture.
   */
  std::unique_ptr<Frame>
  capture();

  /** \brief Stream Motion JPEG.
   *  \pre The camera has been initialized to JPEG mode.
   *  \return number of frames streamed.
   */
  int streamMjpeg(Client& client, const StreamMjpegConfig& cfg);
  int streamMjpeg(Client& client) {
    return streamMjpeg(client, StreamMjpegConfig());
  }

  /** \brief Stream Motion JPEG.
   *  \Function cloned for AsyncClient
   *  \pre The camera has been initialized to JPEG mode.
   *  \return number of frames streamed.
   */
  int streamMjpeg(AsyncClient& client, const StreamMjpegConfig& cfg);
  int streamMjpeg(AsyncClient& client) {
    return streamMjpeg(client, StreamMjpegConfig());
  }


};

extern CameraClass Camera;

inline std::unique_ptr<Frame>
capture()
{
  return Camera.capture();
}

} // namespace esp32cam

#endif // ESP32CAM_H
