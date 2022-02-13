# GoDisplay: camera image display for ODROID-GO

This example runs on [ODROID-GO](https://wiki.odroid.com/odroid_go/odroid_go) game console.
It displays images captured by ESP32-CAM (running WifiCam example) and retrieved over HTTP.
To use this example, modify WiFi SSID+password and ESP32-CAM IP address, then upload to ODROID-GO.

As of 2022-02-06, [ODROID-GO package](https://github.com/hardkernel/ODROID-GO) is incompatible with ESP32 Arduino core v2.0.x.
Deleting `ODROID-GO/src/sensors` and `ODROID-GO/src/web` directories would resolve the compile errors.
