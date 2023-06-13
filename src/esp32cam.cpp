#include "esp32cam.h"

#include <Arduino.h>
#include <esp_camera.h>

#define BOUNDARY "e8b8c539-047d-4777-a985-fbba6edff11e"
// #undef BOUNDARY

namespace esp32cam {

CameraClass Camera;

bool
CameraClass::begin(const Config& config)
{
  lastEspErr = esp_camera_init(reinterpret_cast<const camera_config_t*>(config.m_cfg));
  return (lastEspErr == ESP_OK ? true : false);
}

bool
CameraClass::end()
{
  lastEspErr = esp_camera_deinit();
  return (lastEspErr == ESP_OK ? true : false);
}

bool
CameraClass::changeResolution(const Resolution& resolution, int sleepFor)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  framesize_t frameSize = resolution.as<framesize_t>();
  if (sensor->status.framesize == frameSize) {
    return true;
  }

  if (sensor->set_framesize(sensor, frameSize) != 0) {
    return false;
  }
  delay(sleepFor);
  return true;
}

bool
CameraClass::changeFrequence(int iMhz)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }
  
  int iLedTimer = 0;
  int ret = 0;
  sensor->xclk_freq_hz = iMhz * 1000000U;
//  ret = xclk_timer_conf(iLedTimer, sensor->xclk_freq_hz);
  // if (sensor->set_xclk(sensor, iLedTimer, iMhz) != 0) {
  if (ret  != 0) {
    return false;
  } else 
	return true;
}

bool
CameraClass::changeContrast(int ilevel)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_contrast(sensor, ilevel) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changeBrightness(int ilevel)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_brightness(sensor, ilevel) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changeSaturation(int ilevel)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_saturation(sensor, ilevel) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changeSpecialEffect(int ilevel)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_special_effect(sensor, ilevel) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changeWbMode(int ilevel)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_wb_mode(sensor, ilevel) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changeAeLevels(int ilevel)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_ae_level(sensor, ilevel)) {
    return true;
  } else
    return false;
}

bool
CameraClass::changAgcGain(int ilevel)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_agc_gain(sensor, ilevel) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changGainceilingSensor(int iGainCeiling)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if ((gainceiling_t)iGainCeiling < GAINCEILING_2X ||
      (gainceiling_t)iGainCeiling > GAINCEILING_128X) {
    return false;
  }

  if (sensor->set_gainceiling(sensor, (gainceiling_t)iGainCeiling) == 0) {
    return true;
  } else
    return false;
}
   
bool
CameraClass::changeAwbGainControl(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_awb_gain(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}


bool
CameraClass::changeGaincontrol(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_gain_ctrl(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changeColorbar(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_colorbar(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changeWhitebalance(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_whitebal(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changeQuality(int iQuality)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (iQuality < 0 ||
      iQuality > 63) {
    return false;
  }

  if (sensor->set_quality(sensor, iQuality) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changAec2(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_aec2(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changeAecValue(int iValue)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }
  
  if (iValue < 0 ||
      iValue > 1200) {
    return false;
  }

  if (sensor->set_aec_value(sensor, iValue) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changHMirror(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_hmirror(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changVFlip(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_vflip(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changBPC(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_bpc(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changWPC(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_wpc(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changDenoise(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_denoise(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changLenc(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_lenc(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changRawGMA(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_raw_gma(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changAutoExposurecontrol(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_exposure_ctrl(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changDcw(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  if (sensor->set_dcw(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

bool
CameraClass::changeZoom(int iEnable)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }

  // sensor->status.aec2 = enable;
  if (sensor->set_reg(sensor, 0x12 /*COM7*/, 0xFF, iEnable?0x04/*COM7_ZOOM_EN*/:0) == 0) {
//  if (sensor->set_reg(sensor, 0x01 /*BANK_SENSOR*/, 0x12 /*COM7*/, iEnable?0x04/*COM7_ZOOM_EN*/:0) == 0) {
//  if (sensor->set_zoom(sensor, iEnable) == 0) {
    return true;
  } else
    return false;
}

int
CameraClass::getBandMode() 
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }
  return sensor->get_reg(sensor, 0x3C0C, 0x01);
}
  
bool
CameraClass::changeBandMode(int ilevel) 
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
	return false;
  }
  // OV3660 registers
  if ( ilevel == 0 ) {
	// Auto Mode: Enable 0x3004[2]
	if (sensor->set_reg(sensor, 0x3a00, 0x32, 0x32) == 0 && 
		sensor->set_reg(sensor, 0x3004, 0x04, 0x04) == 0 && 
		sensor->set_reg(sensor, 0x3c00, 0x04, 0x00) == 0 /* 60Hz? */ )
		return true;
	else
		return false;
  } else {
	// Manual Mode: Disable registers
  	if (sensor->set_reg(sensor, 0x3a00, 0x32, 0x00) == 0 && 
		sensor->set_reg(sensor, 0x3004, 0x04, 0x00) == 0 ) {
		if ( ilevel == 1 ) {
// TESTING sensor->set_reg(sensor, 0x5000, 0x20, 0x20);
			// 50 HZ
			if (sensor->set_reg(sensor, 0x3c00, 0x04, 0x04) == 0) {
				sensor->set_reg(sensor, 0x3a08, 0x00, 0x00);//50HZ BAND WIDTH
				sensor->set_reg(sensor, 0x3a09, 0xeb, 0xeb);//50HZ BAND WIDTH
				sensor->set_reg(sensor, 0x3a0e, 0x06, 0x06);//50HZ MAX BAND

				sensor->set_reg(sensor, 0x3C0C, 0x01, 0x01);
				return true;
				} else
				return false;
		} else {
// TESTING sensor->set_reg(sensor, 0x5000, 0x20, 0x00);
			// 60 HZ
			if (sensor->set_reg(sensor, 0x3c00, 0x04, 0x00) == 0) {
				sensor->set_reg(sensor, 0x3a0a, 0x00, 0x00);//60HZ BAND WIDTH
				sensor->set_reg(sensor, 0x3a0b, 0xc4, 0xc4);//60HZ BAND WIDTH
				sensor->set_reg(sensor, 0x3a0d, 0x08, 0x08);//60HZ MAX BAND
				
				sensor->set_reg(sensor, 0x3C0C, 0x01, 0x00);
				return true;
			} else
				return false;
		}		
	} else {
		return false;
	}
  }

  /* OV3660
	{0x3a00, 0x38}, //BIT[5]:1,banding function enable;bit[2] night mode disable
	// /******if use 50HZ banding remove, refer to below setting********
	{0x3c00, 0x04}, //BIT[2]:1,ENABLE 50HZ
	{0x3a14, 0x06}, //NIGHT MODE CEILING, 50HZ
	{0x3a15, 0x6d}, //NIGHT MODE CEILING, 50HZ
	{0x3a08, 0x00}, //50HZ BAND WIDTH
	{0x3a09, 0xeb}, //50HZ BAND WIDTH
	{0x3a0e, 0x06}, //50HZ MAX BAND
	
	{0x380e, 0x06}, 
	{0x380f, 0x6d}, //inset 81 dummy lines for banding filter//1c
	// /******if use 60HZ banding remove, refer to below setting********
	
	//{0x3c00, 0x00}, //BIT[2]:0,ENABLE 50HZ
	{0x3a02, 0x06}, //NIGHT MODE CEILING, 60HZ
	{0x3a03, 0x6d}, //NIGHT MODE CEILING, 60HZ
	{0x3a0a, 0x00}, //60HZ BAND WIDTH
	{0x3a0b, 0xc4}, //60HZ BAND WIDTH
	{0x3a0d, 0x08}, //60HZ MAX BAND
	
	0x3004[2] = 1 = AUTO
	0x3004[2] = 0 = MANUAL
	0X3C0C[0] = 0 = 60hz
	0X3C0C[0] = 1 = 50hz
   */
}

bool
CameraClass::changePll(int iMul, int iPre, int iPclk)
{
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return false;
  }
	
  if (sensor->set_pll(	sensor, 
						false, 
						iMul, 
						1, 
						iPre, 
						false, 
						0, 
						true, 
						iPclk) == 0) {
    return true;
  } else
    return false;
}


std::unique_ptr<Frame>
CameraClass::capture()
{
  camera_fb_t* fb = esp_camera_fb_get();
  if (fb == nullptr) {
    return nullptr;
  }
  return std::unique_ptr<Frame>(new Frame(fb));
}

int 
CameraClass::streamMjpeg(Client& client, const StreamMjpegConfig& cfg) {
  client.print("HTTP/1.1 200 OK\r\n"
               "Content-Type: multipart/x-mixed-replace;boundary=" BOUNDARY "\r\n"
               "\r\n");
  auto lastCapture = millis();
  int nFrames;
  for (nFrames = 0; cfg.maxFrames < 0 || nFrames < cfg.maxFrames; ++nFrames) {
    auto now = millis();
    auto sinceLastCapture = now - lastCapture;
    if (static_cast<int>(sinceLastCapture) < cfg.minInterval) {
      delay(cfg.minInterval - sinceLastCapture);
    }
    lastCapture = millis();

    auto frame = capture();
    if (frame == nullptr) {
      break;
    }

    client.printf("Content-Type: image/jpeg\r\n"
                  "Content-Length: %d\r\n"
                  "\r\n",
                  static_cast<int>(frame->size()));
    if (!frame->writeTo(client, cfg.frameTimeout)) {
      break;
    }
    client.print("\r\n--" BOUNDARY "\r\n");
  }
  return nFrames;
}

int 
CameraClass::streamMjpeg(AsyncClient& client, const StreamMjpegConfig* cfg) {
  client.write("HTTP/1.1 200 OK\r\n"
               "Content-Type: multipart/x-mixed-replace;boundary=" BOUNDARY "\r\n"
               "\r\n");
  auto lastCapture = millis();
  int nFrames;
  int nNullFrames = 0;
  for (nFrames = 0; cfg->maxFrames < 0 || nFrames < cfg->maxFrames; ++nFrames) {
    auto now = millis();
    auto sinceLastCapture = now - lastCapture;
    if (static_cast<int>(sinceLastCapture) < cfg->minInterval) {
      delay(cfg->minInterval - sinceLastCapture);
    }
    lastCapture = millis();

    auto frame = capture();
    if (frame == nullptr) {
	  nNullFrames++;
	  if (nNullFrames > 10)
		  break;
	  else {
		  continue;
	  }
    } else {
		nNullFrames = 0;
	}

    char szTmp[256];
    size_t stLen = snprintf_P(szTmp, 256, PSTR(
	"Content-Type: image/jpeg\r\n"
	"Content-Length: %d\r\n"
	"\r\n"), static_cast<int>(frame->size())
	);
	if (!client.disconnected()) {
		client.write((const char*) szTmp, stLen);
		if (!frame->writeTo(client, cfg->frameTimeout)) {
		  break;
		}
	} else {
	  break;
	}
    client.write("\r\n--" BOUNDARY "\r\n");
    taskYIELD();
	
	if (cfg->bAbort)
		break;
  }
  return nFrames;
}

} // namespace esp32cam
